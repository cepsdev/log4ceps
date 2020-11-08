
#ifndef INC_LOG4CEPS_DYNBITSET_HPP
#define INC_LOG4CEPS_DYNBITSET_HPP

#include <memory>
#include <cstring>
#include <iostream>

namespace log4ceps {
 class Dynamic_bitset{
	 std::unique_ptr<char> arry_;
	 size_t valid_bits_ = 0;
	 size_t arry_size_ = 0;

	 std::size_t compute_chunk_size_in_chars(std::size_t s){
		 return s <= 0 ? 1 : (s+7) >> 3;
	 }
	 public:
	 Dynamic_bitset() {}
	 Dynamic_bitset(int bits, const char* chunk){
		 arry_.reset(new char[arry_size_ = compute_chunk_size_in_chars(valid_bits_=bits)]);
		 std::memcpy(arry_.get(),chunk,arry_size_);
	 }
	 Dynamic_bitset(int bits){
		 arry_.reset(new char[arry_size_ = compute_chunk_size_in_chars(valid_bits_=bits)]);
		 std::memset(arry_.get(),0,arry_size_);
	 }
	 Dynamic_bitset(const Dynamic_bitset& rhs){
		 arry_size_=rhs.arry_size_;
		 valid_bits_=rhs.valid_bits_;
		 if (rhs.arry_size_==0) return;
		 arry_.reset(new char[arry_size_]);
		 std::memcpy(arry_.get(),rhs.arry_.get(),arry_size_);
	 }
	 Dynamic_bitset& operator=(const Dynamic_bitset& rhs){
		 arry_.release();
		 arry_size_=rhs.arry_size_;
		 valid_bits_=rhs.valid_bits_;
		 if (rhs.arry_size_==0) return *this;
		 arry_.reset(new char[arry_size_]);
		 std::memcpy(arry_.get(),rhs.arry_.get(),arry_size_);
		 return *this;
	 }
	 size_t allocated_memory_size() const {return arry_size_;}	 
	 bool operator[](int i) const { return get(i) ; }
	 int get(int i) const{return arry_.get()[i >> 3] & ( 1 << (i % 8) ) ;}
	 void reset(size_t i) {arry_.get()[i >> 3] =  arry_.get()[i >> 3] & ~( 1 << (i % 8) )  ; }	 
	 template<typename Cont> void set(Cont const & v){
		 std::memset(arry_.get(),0,arry_size_);
		 for(auto c : v){
			 set(c);
		 }
	 }
	 
	 size_t size() const {return valid_bits_;}
	 void set(size_t i) {arry_.get()[i >> 3] =  arry_.get()[i >> 3] | ( 1 << (i % 8) )  ; }	 
	 void set(int i) {arry_.get()[i >> 3] =  arry_.get()[i >> 3] | ( 1 << (i % 8) )  ; }
	 char* data() const {return arry_.get();}	
	 char* data() {return arry_.get();}
	 void clear() {std::memset(arry_.get(),0,arry_size_);}
	 friend size_t deserialize_value(log4ceps::Dynamic_bitset & , char *, size_t );
 };
 
 std::ostream& operator << (std::ostream& os,Dynamic_bitset const & b );
}

#endif
