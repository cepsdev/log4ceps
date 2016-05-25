/* LOG4KMW_DEFAULT_LOG_READER.CPP 
   CREATED Tue May 24 15:43:11 2016

   GENERATED BY logspec2cpp VERSION 0.5 (c) Krauss-Maffei-Wegmann GmbH & Co. KG. 
   BASED ON CEPS VERSION 1.1 (Apr 30 2016) BUILT WITH GCC 4.9.0 on GNU/LINUX 64BIT (C) BY THE AUTHORS OF ceps (ceps is hosted at github: https://github.com/cepsdev/ceps.git) 
   THIS IS A GENERATED FILE. DO NOT MODIFY.
*/


#include <iostream>
#include <type_traits>
#include <tuple>
#include <cstdint>
#include <memory>
#include <iterator>
#include <limits>
#include <cstdlib>
#include <time.h>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include "log4kmw_states.hpp"
#include "log4kmw_records.hpp"
#include "log4kmw_loggers.hpp"

#ifdef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#else
#endif
using namespace log4kmw;

int main(int argc, char** argv)
{
 using namespace std;
 using namespace log4kmw;
 using namespace log4kmw_loggers;
 using namespace log4kmw_events;
 
 if(argc == 1)
 {
   cerr << "\n\nlog4kmw reader.\n\nUsage :" << argv[0] << " FILE [FILE...]\nWhere FILE is a logfile generated by a compatible logwriter.\n\n"; 
   return 1;
 }

 
 for (auto i = 1; i < argc; ++i)
 {
   struct stat sb = {0};
   if (-1 == stat(argv[i],&sb))
   {
     cerr << "\n***Fatal Error: Couldn't retrieve information about '"<< argv[i] << "'.\n\n";
     return 2;
   }
   logger_status_all.init(log4kmw::persistence::memory_mapped_file(argv[i], sb.st_size, false));
   int counter = 1;
   for (
		auto it = log4kmw_loggers::logger_status_all.cbegin(); // get iterator pointing to the very first entry in the logbook
		it != log4kmw_loggers::logger_status_all.cend();       // loop while we haven't reached the end
		++it		   
		)
	{
        std::cout << "LOG ENTRY #"<<counter++<<":\n"; 
		std::cout << (*it).states() << std::endl;
	}
   
 }
 

}
