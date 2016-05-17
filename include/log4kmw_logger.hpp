/*
Copyright 2015, Krauss-Maffei-Wegmann GmbH & Co. KG.
All rights reserved.

@author Tomas Prerovsky <tomas.prerovsky@gmail.com>

Description:
Ringbuffer.
*/


#ifndef INC_LOG4KMW_LOGGER_HPP

#define INC_LOG4KMW_LOGGER_HPP

#include "log4kmw_si_units.hpp"
#include "log4kmw_serialization.hpp"
#include "log4kmw_state.hpp"
#include "log4kmw_record.hpp"
#include "log4kmw_ringbuffer.hpp"


#include <iostream>


#ifdef _WIN32
#include <Windows.h>
#define __attribute__(A)
#endif

#ifndef _WIN32
 #ifdef __linux

 #include <sys/mman.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <unistd.h>
 #include <errno.h>
 #include <string.h>
 #include <stdio.h>
 #endif
#endif


template <typename F>
struct Always_call_on_exit_struct
{
  F& f_;
  Always_call_on_exit_struct(F& f):f_(f) {}
  ~Always_call_on_exit_struct() {}
};

template<typename F> Always_call_on_exit_struct<F>  call_on_exit(F& f)
{
 return {f};
}

template<typename C,typename R, typename... Ts> int  call_on_exit( R (C::* f )(Ts...) , C* c)
{

 return 0;
}

#if 0
void g(){}
#endif
namespace log4kmw {

	namespace persistence {
		struct in_memory{
			char * data;
			size_t size;
			bool unintialized_data;
		};

		#ifdef _WIN32
		class memory_mapped_file
		{
			HANDLE fh_ = INVALID_HANDLE_VALUE;
			HANDLE mapping_object_ = INVALID_HANDLE_VALUE;
			bool uninitialized_file_ = false;
			void * map_view_ = nullptr;
			size_t map_view_size_ = 0;

			void cleanup()
			{
				if (fh_ != INVALID_HANDLE_VALUE) CloseHandle(fh_);
				if (mapping_object_ != INVALID_HANDLE_VALUE && mapping_object_ != NULL) 
					CloseHandle(mapping_object_);
				if (map_view_) UnmapViewOfFile(map_view_);
			}
		public:
			memory_mapped_file() = default;
			memory_mapped_file(memory_mapped_file const &) = delete;
			memory_mapped_file(memory_mapped_file && mmf)
			{
				fh_ = mmf.fh_;
				mmf.fh_ = INVALID_HANDLE_VALUE;
				map_view_ = mmf.map_view_;
				mmf.map_view_ = nullptr;
				map_view_size_ = mmf.map_view_size_;
				uninitialized_file_ = mmf.uninitialized_file_;
			}
			memory_mapped_file & operator = (memory_mapped_file && mmf)
			{
				cleanup();
				fh_ = mmf.fh_;
				mmf.fh_ = INVALID_HANDLE_VALUE;
				map_view_ = mmf.map_view_;
				mmf.map_view_ = nullptr;
				map_view_size_ = mmf.map_view_size_;
				uninitialized_file_ = mmf.uninitialized_file_;
				return *this;
			}

			void init(std::string const & path, size_t total_size, bool overwrite = false)
			{
			    //@TODO: make cleanup exception safe
				uninitialized_file_ = overwrite;
				if (INVALID_HANDLE_VALUE ==
					(fh_ =
					CreateFile(path.c_str(),
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					overwrite ? CREATE_ALWAYS : OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL)
					)
					)
					throw std::runtime_error("Couldn't create file: " + path);
				DWORD file_size = 0;
				GetFileSize(fh_, &file_size);
				if (file_size != total_size)
				{
					SetFilePointer(fh_, total_size, 0, FILE_BEGIN);
					SetEndOfFile(fh_);
					uninitialized_file_ = true;
				}
				map_view_size_ = total_size;
				mapping_object_ = CreateFileMapping(
					fh_,
					NULL,
					PAGE_READWRITE,
					0,
					0,
					NULL
					);
				if (mapping_object_ == NULL)
				{
					cleanup();
					throw std::runtime_error("Couldn't create memory mapping for file: " + path);
				}

				map_view_ = MapViewOfFile(
					mapping_object_,
					FILE_MAP_ALL_ACCESS,
					0, 0, 0);
				if (map_view_ == NULL)
				{
					cleanup();
					throw std::runtime_error("Couldn't create memory mapping (MapViewOfFile failed).");
				}
				else { CloseHandle(mapping_object_); mapping_object_ = NULL; }
			}

			memory_mapped_file(std::string const & path, size_t total_size, bool overwrite = false)
			{
				init(path,total_size, overwrite);
			}

			void * map_addr() const { return map_view_; }
			size_t size() const { return map_view_size_; }
			bool initialized() const { return !uninitialized_file_; }
		
			~memory_mapped_file()
			{
				cleanup();
			}
		};
		#endif

		#ifdef __linux
		class memory_mapped_file
		{

			bool uninitialized_file_ = false;
			int fd_ = -1;
			void* mapped_mem_ = nullptr;
			std::int64_t map_view_size_ = 0;

			void cleanup()
			{
			  if (mapped_mem_ != nullptr && map_view_size_ > 0)
			  {
			    msync(mapped_mem_,map_view_size_,MS_SYNC);
			    munmap(mapped_mem_,map_view_size_);
			    mapped_mem_ = nullptr;
			    map_view_size_=0;
			  }
			  if (fd_ != -1) {close(fd_);fd_=-1;}

			}
		public:
			memory_mapped_file() = default;
			memory_mapped_file(memory_mapped_file const &) = delete;
			memory_mapped_file(memory_mapped_file && mmf)
			{
				fd_ = mmf.fd_;
				mmf.fd_ = -1;
				mapped_mem_  = mmf.mapped_mem_;
				mmf.mapped_mem_ = nullptr;
				map_view_size_ = mmf.map_view_size_;
				uninitialized_file_ = mmf.uninitialized_file_;
			}
			memory_mapped_file & operator = (memory_mapped_file && mmf)
			{
				cleanup();
				fd_ = mmf.fd_;
				mmf.fd_ = -1;
				mapped_mem_  = mmf.mapped_mem_;
				mmf.mapped_mem_ = nullptr;
				map_view_size_ = mmf.map_view_size_;
				uninitialized_file_ = mmf.uninitialized_file_;
				return *this;
			}

			void init(std::string const & path, size_t total_size, bool overwrite = false)
			{
			  //@TODO: make cleanup exception safe
			  uninitialized_file_ = overwrite;

			  fd_ = open(path.c_str(),O_RDWR | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

			  if (fd_ == -1)
			   throw std::runtime_error("Couldn't open file: " + path);

			  std::int64_t file_size = 0;
			  {
			    struct stat sb;
			    if(fstat(fd_,&sb) == -1)
			     throw std::runtime_error("fstat failed.");
			    file_size = sb.st_size;
			  }


			  if (file_size != total_size)
			    {
				ftruncate(fd_,total_size);
				uninitialized_file_ = true;
			    }
			   map_view_size_ = total_size;

			  mapped_mem_ = mmap(nullptr,map_view_size_,PROT_READ | PROT_WRITE,MAP_SHARED,fd_,0);
			  if (mapped_mem_ == MAP_FAILED)
			  {
			    std::string err_text = strerror(errno);
			    cleanup();
			    throw std::runtime_error("Couldn't create memory mapping (mmap failed):"+ err_text);
			  }
			}

			memory_mapped_file(std::string const & path, size_t total_size, bool overwrite = false)
			{
			  init(path,total_size, overwrite);
			}

			void * map_addr() const { return mapped_mem_; }
			size_t size() const { return map_view_size_; }
			bool initialized() const { return !uninitialized_file_; }

			~memory_mapped_file()
			{
				cleanup();
			}
		};
		#endif
	}

	template<typename Rec, typename Persistencylayer>
	class Logger : public Persistencylayer, public Ringbuffer<Rec>
	{
		private:
			bool invalid_ = true;
		public:

			Logger() {}

			Logger(persistence::in_memory pl)
				:Persistencylayer(pl),
				 Ringbuffer<Rec>(pl.data + sizeof(Ringbufferheader),
				 pl.data,
				 pl.size - sizeof(Ringbufferheader),
				 pl.unintialized_data)
			{
				invalid_ = false;
			}

			Logger(persistence::memory_mapped_file&& pl)
				:Persistencylayer(std::move(pl)),
				 Ringbuffer<Rec>((char*)Persistencylayer::map_addr() + sizeof(Ringbufferheader),
				 (char*)Persistencylayer::map_addr(),
				 Persistencylayer::size() - sizeof(Ringbufferheader),
				 !Persistencylayer::initialized() )
			 
			{
				invalid_ = false;
			}

			void init(persistence::memory_mapped_file&& pl)
			{
				*static_cast<persistence::memory_mapped_file*>(this) = std::move(pl);
				*static_cast<Ringbuffer<Rec>*>(this) =
					Ringbuffer<Rec>(
						(char*)Persistencylayer::map_addr() + sizeof(Ringbufferheader),
						(char*)Persistencylayer::map_addr(),
						Persistencylayer::size() - sizeof(Ringbufferheader),
						!Persistencylayer::initialized()
					);
				invalid_ = false;
			}

			operator bool() const { return !invalid_; }
	
			Ringbuffer<Rec> const & ring_buffer() const { return *this;/*ring_buffer_*/; }
	};



}//namespace log4kmw

#endif
