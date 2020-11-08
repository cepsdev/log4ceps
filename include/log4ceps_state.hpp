#ifndef INC_LOG4CEPS_STATE_HPP
#define INC_LOG4CEPS_STATE_HPP

#include "log4ceps_si_units.hpp"
#include "log4ceps_serialization.hpp"
#include <iostream>
#include <tuple>
#include <time.h>

#ifdef _WIN32
 #define __attribute__(A)
#endif

std::ostream & operator << (std::ostream & os, timespec const &);

namespace log4ceps {

	template<typename... T> class State;//general declaration of template class State
	
	/**
	* The empty state case.
	*/
	template<> 
	class State < >
	{
		public:
			State() = default;
			State& operator = (const State & )
			{
				return *this;
			}
			bool operator == (State const & ) const
			{
				return true; //The empty state is uniquely defined.
			}

			bool operator != (State const & ) const
			{
				return false; //The empty state is uniquely defined.
			}


			size_t serialize(char *,
				size_t,
				bool,
				log4ceps::throw_exception_policy ) const
			{
				return 0;
			}

			size_t serialize(char,
				size_t,
				bool,
				log4ceps::nothrow_exception_policy) const
			{
				return 0;
			}

			size_t deserialize(char *,
				size_t)
			{
				return 0;
			}
	};
	
	/**
	* State class. The State class is a variadic template.
	*/
	template<typename Head, typename... Tail>
	class State<Head,Tail...> : private State < Tail... >
	{
		public:
			typedef Head head_type;
			typedef Head first_type;
			typedef State<Tail...> second_type;
			typedef State<Tail...> tail_type;

			State() = default;
			State(const Head & v, const Tail & ... vtail) :tail_type(vtail...), head_(v)
			{}
			template <typename Head_rhs, typename... Tail_rhs>
			State(State<Head_rhs, Tail_rhs...> const & rhs) 
				: tail_type(rhs.second()),head_(rhs.first())
			{
			}
			second_type & second() { return *this; }
			second_type const & second() const { return *this; }
			second_type & inherited() { return *this; }
			second_type const & inherited() const { return *this; }
			head_type & first() { return head_; }
			head_type const & first() const { return head_; }
			head_type & head() { return head_; }
			head_type const & head() const { return head_; }

		protected:
			Head head_;
		public:


			template <typename Head_rhs>
			State& operator = (typename std::enable_if<sizeof...(Tail) == 0, Head_rhs>::type  const & rhs_head) {
				head_ = rhs_head;
				return *this;
			}

			
			template <typename Head_rhs, typename... Tail_rhs>
			State& operator = (State<Head_rhs, Tail_rhs...> const & rhs) {
				head_ = rhs.first();
				second() = rhs.second();
				return *this;
			}

			bool operator == (State const & rhs) const
			{
				return head_ == rhs.head_ && second() == rhs.second();
			}

			bool operator != (State const & rhs) const
			{
				return head_ != rhs.head_ || second() != rhs.second();
			}
		
			size_t serialize(char * buffer, 
				             size_t size, 
							 bool write_data, 
							 log4ceps::throw_exception_policy) const
			{
				auto t = serialize_value(first(), buffer, size, write_data, log4ceps::throw_exception_policy());
				return t + State<Tail...>::serialize(buffer + t, size - t, write_data, log4ceps::throw_exception_policy());
			}

			size_t serialize(char * buffer, 
							 size_t size, 
							 bool write_data, 
							 log4ceps::nothrow_exception_policy) const
			{
				auto t = serialize_value(head_, buffer, size, write_data, log4ceps::nothrow_exception_policy());
				if (t == 0) return 0;
				return t + State<Tail...>::serialize(buffer + t, size - t, write_data, log4ceps::throw_exception_policy());
			}

			size_t deserialize(char * buffer, 
							   size_t size)
			{
				auto t = deserialize_value(head_, buffer, size);
				return t + State<Tail...>::deserialize(buffer + t, size - t);
			}

			size_t size() const
			{
				return inherited().size() + 1;
			}		
	};


	
	template <int N, typename... T>
	class Type_of_Nth_element;

	template<int N, typename Head, typename... Tail>
	class Type_of_Nth_element<N, Head, Tail...>
	{
	public:
		typedef typename Type_of_Nth_element<N - 1, Tail...>::type type;
	};

	template< typename Head, typename... Tail>
	class Type_of_Nth_element < 0, Head, Tail...>
	{
	public:
		typedef Head type;
	};

	template<int N, typename Head, typename... Tail>
	struct peel_off :public peel_off<N - 1, Tail...>
	{
		peel_off(State<Head, Tail...> & s) :peel_off<N - 1, Tail...>(s.inherited()) {}
	};

	template<typename Head, typename... Ts>
	struct peel_off<0, Head, Ts...>
	{
		Head& value;
		peel_off(State<Head, Ts...> & s) : value(s.head()) {}

	};

	template <int N, typename... Ts>
	typename Type_of_Nth_element<N, Ts...>::type & get_value(State< Ts...>& s)
	{
		return peel_off<N, Ts...>(s).value;
	}


	template<int N, typename Head, typename... Tail>
	struct peel_off_const :public peel_off_const<N - 1, Tail...>
	{
		peel_off_const(State<Head, Tail...> const & s) :peel_off_const<N - 1, Tail...>(s.inherited()) {}
	};

	template<typename Head, typename... Ts>
	struct peel_off_const<0, Head, Ts...>
	{
		Head const& value;
		peel_off_const(State<Head, Ts...> const & s) : value(s.head()) {}

	};

	template <int N, typename... Ts>
	typename Type_of_Nth_element<N, Ts...>::type const & get_value(State< Ts...> const& s)
	{
		return peel_off_const<N, Ts...>(s).value;
	}

	// Print routines

	template<typename... Ts>
	std::ostream & operator << (std::ostream & os, State< Ts... > const & s);


	template<typename T, typename... Ts>
	std::ostream & operator << (std::ostream & os, State<T, Ts... > const & s)
	{
		os << s.head();
		os << " " << s.inherited();
		return os;
	}

	template<typename... Ts>
	std::ostream & operator << (std::ostream & os, State< Ts... > const &)
	{

		return os;
	}

}


#endif
