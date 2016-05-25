/*
Copyright 2015, Krauss-Maffei-Wegmann GmbH & Co. KG.
All rights reserved.

@author Tomas Prerovsky <tomas.prerovsky@gmail.com>


*/


#include "log4kmw_dynamic_bitset.hpp"
#include <string>
#include <ostream>
std::ostream& log4kmw::operator << (std::ostream& os,log4kmw::Dynamic_bitset const & b ){
	 for(size_t i = 0; i != b.size();++i)
		 if (b.get(i)) os << "1"; else os << "0";
	 return os;
}
