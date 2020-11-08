#include "log4ceps_dynamic_bitset.hpp"
#include <string>
#include <ostream>
std::ostream& log4ceps::operator << (std::ostream& os,log4ceps::Dynamic_bitset const & b ){
	 for(size_t i = 0; i != b.size();++i)
		 if (b.get(i)) os << "1"; else os << "0";
	 return os;
}
