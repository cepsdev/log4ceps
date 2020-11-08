#ifndef INC_LOG4CEPS_RINGBUFFER_HPP
#define INC_LOG4CEPS_RINGBUFFER_HPP

#include "log4ceps_si_units.hpp"
#include "log4ceps_serialization.hpp"
#include "log4ceps_state.hpp"
#include "log4ceps_record.hpp"
#include <cstdint>
#include <iostream>
#include <memory>

namespace log4ceps {
	struct Ringbufferheader
	{
		std::int64_t start; // points to the very first valid byte of the first record
		std::int64_t end;   // points to the last byte of the last record
		std::int64_t wrap_around_ofs;
		std::int64_t wrap_around_size;
	};

	class Ringbufferexception : public std::runtime_error
	{
	public:
		Ringbufferexception(const std::string & what) : std::runtime_error(what) {}
	};

	template<typename R> class Ringbuffer
	{
		char * data_ = nullptr;
		char * header_p_ = nullptr;
		size_t capacity_ = 0;


		bool data_owner_ = false;
		bool header_owner_ = false;


		void append_record(R const & rec, size_t size, Ringbufferheader& rh)
		{
			//INVARIANT: The buffer contains at least size bytes of free space
			if (header().end + size < capacity_)
			{
				serialize(rec, data_ + header().end + 1, size);
				rh.end += size;
			}
			else if (capacity_ - (header().end + 1) == 0)
			{
				serialize(rec, data_, size);
				rh.end = size - 1;
			}
			else //wrap around boundary
			{
				char * buffer;
				std::unique_ptr<char> buffer_handle(buffer = new char[size]); //delete will be automatically called at end of this scope
				serialize(rec, buffer, size);
				auto prefix_size = capacity_ - (header().end + 1);
				//INVARIANT: prefix_size > 0
				memcpy(data_ + header().end + 1, buffer,
					static_cast<size_t>(prefix_size));
				memcpy(data_, buffer + prefix_size, static_cast<size_t>(size - prefix_size));
				rh.wrap_around_ofs = header().end + 1;
				rh.wrap_around_size = size;
				rh.end = size - prefix_size - 1;
			}
		}

		void update_header(Ringbufferheader& rh)
		{
			header() = rh;
		}

		bool pop(Ringbufferheader& rh)
		{
			if (rh.start == 0 && rh.end == -1) return false;
			bool check_for_wraparound = rh.wrap_around_size > 0;
			if (check_for_wraparound && rh.start == rh.wrap_around_ofs)
			{
				rh.start = rh.wrap_around_size - (capacity_ - rh.wrap_around_ofs);
				rh.wrap_around_size = 0;
				return true;
			}
			R rec;
			auto size = deserialize(rec, data_ + rh.start, static_cast<size_t>(capacity_ - rh.start));
			rh.start += size;
			if (rh.start >= static_cast<int64_t>(capacity_) ) rh.start -= capacity_;
			if (rh.end + 1 == rh.start)
			{
				rh.start = 0; rh.end = -1;
			}
			return true;
		}

		void cleanup()
		{
			if (data_owner_) delete[] data_;
			if (header_owner_) delete[] header_p_;
		}

		std::int64_t next(std::int64_t ofs) const
		{
			if (ofs == -1) return -1;

			if (header().start == 0 && header().end == -1) return -1;
			if (header().wrap_around_size > 0 && header().wrap_around_ofs == ofs)
			{
				std::int64_t t = header().wrap_around_size - (capacity_ - header().wrap_around_ofs);
				if (t == header().end + 1) return -1;
				return t;
			}
			R rec;
			std::int64_t size = deserialize(rec, data_ + ofs, static_cast<size_t>(capacity_ - ofs + 1));
			ofs += size;
			if (ofs >= static_cast<int64_t>(capacity_)) ofs -= capacity_;
			if (ofs == header().start) return -1;
			if (ofs - 1 == header().end) return -1;
			return ofs;
		}

		R get(std::int64_t ofs) const
		{
			R rec;
			if (header().wrap_around_size > 0 && header().wrap_around_ofs == ofs)
			{
				char * buffer;
				std::unique_ptr<char> buffer_handle(buffer = new char[(size_t)header().wrap_around_size]);
				size_t prefix_size = (size_t)(capacity() - header().wrap_around_ofs);
				memcpy(buffer, data_ + header().wrap_around_ofs, prefix_size);
				memcpy(buffer + prefix_size, data_, (size_t)(header().wrap_around_size - prefix_size));
				deserialize(rec, buffer, (size_t)header().wrap_around_size);
				return rec;
			}
			deserialize(rec, data_ + ofs, static_cast<size_t>(capacity_ - ofs + 1));
			return rec;
		}

	public:
		Ringbuffer(char * data, char * header, size_t capacity, bool initialize = false)
			: data_(data), header_p_(header), capacity_(capacity)
		{
			init(data, header, capacity, initialize);
		}

		Ringbuffer() = default;

		void init(char * data, char * header, size_t capacity, bool initialize = false)
		{
		    data_ = data;
		    header_p_ = header;
		    capacity_ = capacity;
		    data_owner_ = false;
		    header_owner_ = false;
			if (initialize)
				clear();
		}

		explicit Ringbuffer(size_t capacity)
		{
			if (capacity < sizeof(R)) throw Ringbufferexception("Invalid Argument: Capacity too low.");
			header_p_ = new char[(capacity_ = capacity) + sizeof(Ringbufferheader)];
			data_ = header_p_ + sizeof(Ringbufferheader);
			header_owner_ = true;
			clear();
		}

		~Ringbuffer()
		{
			cleanup();
		}


		class const_iterator : public std::iterator < std::forward_iterator_tag, R >
		{
			Ringbuffer const & container_;
			std::int64_t ofs_ = -1;
		public:
			const_iterator(Ringbuffer const & container, bool end_it = false) : container_(container)
			{
				if (!end_it) ofs_ = container.header().start;
			}

			bool operator != (const_iterator const & rhs) const
			{
				return ofs_ != rhs.ofs_;
			}

			bool operator == (const_iterator const & rhs) const
			{
				return ofs_ == rhs.ofs_;
			}

			const_iterator& operator ++ ()
			{
				ofs_ = container_.next(ofs_);
				return *this;
			}

			R operator * () const
			{
				return container_.get(ofs_);
			}

		};

		bool empty() const
		{
			return header().start == 0 && header().end == -1;
		}

		const_iterator cbegin() const
		{
			return const_iterator(*this, empty());
		}

		const_iterator cend() const
		{
			return const_iterator(*this, true);
		}


		Ringbufferheader const & header() const
		{
			return *(Ringbufferheader*)header_p_;
		}

		Ringbufferheader & header()
		{
			return *(Ringbufferheader*)header_p_;
		}

		char const * data() const { return data_; }

		void clear()
		{
			header().start = 0;
			header().end = -1;
			header().wrap_around_ofs = 0;
			header().wrap_around_size = 0;
		}

		size_t available_space(Ringbufferheader const & rh) const
		{
			if ((rh.end + 1 == rh.start) && (rh.start == 0)) //Criterium for an empty buffer
				return capacity_;
			if (rh.end >= rh.start)
				return static_cast<size_t>(capacity_ - (rh.end - rh.start + 1));
			return  static_cast<size_t>(rh.start - (rh.end + 1)); // rh.end < rh.start
		}

		size_t capacity() const { return capacity_; }
		size_t allocated_space() const { return capacity() - available_space(header()); }

		void append(R const & rec)
		{
			Ringbufferheader rh = header();

			auto rec_size = serialize(rec, nullptr, 0, false);
			if (rec_size > capacity_) throw Ringbufferexception("insert: Record too large.");
			auto free_space = available_space(rh);

			if (rec_size <= free_space)
			{
				append_record(rec, rec_size, rh);
				update_header(rh);
			}
			else // we have to free some space
			{
				//INVARIANT: rec_size <= capacity_
				for (; pop(rh);) if (rec_size <= available_space(rh)) break;
				// Loop terminates because of invariant
				//INVARIANT: rec_size <= available_space(rh)
				update_header(rh);
				append_record(rec, rec_size, rh);
				update_header(rh);
			}
		}
	};

}//namespace log4kmw

#endif
