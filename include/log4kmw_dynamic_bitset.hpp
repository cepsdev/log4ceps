/*
Copyright 2016, Krauss-Maffei-Wegmann GmbH & Co. KG.
All rights reserved.

Description:
 Dynamic Bitset.
*/
#ifndef INC_LOG4KMW_DYNBITSET_HPP
#define INC_LOG4KMW_DYNBITSET_HPP

#include <memory>
#include <cstring>

namespace log4kmw {
 class Dynamic_bitset{
	 std::unique_ptr<char> arry_;
	 size_t arry_size_;
	 size_t valid_bits_;
	 std::size_t compute_chunk_size_in_chars(std::size_t s){
		 return s <= 0 ? 1 : (s+7) >> 3;
	 }
	 /*class Handle{
		 Dynamic_bitset* parent_;
		 size_t i_;
	 public:
		 Handle(Dynamic_bitset* parent,size_t i):parent_(parent),i_(i){}
		 bool operator == (int i) {return i == parent_->get(i_);}
	 };*/
 public:
	 Dynamic_bitset() = delete;
	 explicit Dynamic_bitset(std::size_t bits){
		 arry_.reset(new char[arry_size_ = compute_chunk_size_in_chars(valid_bits_=bits)]);
		 std::memset(arry_.get(),0,arry_size_);
	 }
	 size_t allocated_memory_size() const {return arry_size_;}	 
	 bool operator[](int i) const { return get(i) ; }
	 /*Handle operator [](int i) {return Handle(this,i);}*/
	 
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
 };
 
 std::ostream& operator << (std::ostream& os,Dynamic_bitset const & b ){
	 for(size_t i = 0; i != b.size();++i)
		 if (b.get(i)) os << "1"; else os << "0";
	 return os;
 }
}

#endif