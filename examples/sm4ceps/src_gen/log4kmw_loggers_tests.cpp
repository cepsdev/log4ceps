/* LOG4KMW_LOGGERS_TESTS.CPP 
   CREATED Wed May 25 14:03:23 2016

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

#include "ceps_all.hh"

/*Start section "Log Printers"*/
namespace log4kmw_test{ namespace meta_info{
 enum base_type{Float,Double,Int,String,Dynamicbitset};
 using state_name=std::string;using value_types_with_units=std::vector<base_type,int/*ceps::ast::Unit_rep*/>;
 std::vector< std::pair<std::string,std::vector<std::pair<base_type,ceps::ast::Unit_rep>>>>Statemachine_log_entry()
 {
  return   {
    std::make_pair(std::string("Current_event"),std::vector<std::pair<base_type,ceps::ast::Unit_rep>>    {
     std::make_pair(base_type::Int,ceps::ast::Unit_rep(0,0,0,0,0,0,0))    }
    ),
    std::make_pair(std::string("Current_states"),std::vector<std::pair<base_type,ceps::ast::Unit_rep>>    {
     std::make_pair(base_type::Dynamicbitset,ceps::ast::Unit_rep(0,0,0,0,0,0,0))    }
    ),
    std::make_pair(std::string("Synch_counter"),std::vector<std::pair<base_type,ceps::ast::Unit_rep>>    {
     std::make_pair(base_type::Int,ceps::ast::Unit_rep(0,0,0,0,0,0,0))    }
    )
  };
 }
 void log_print( log4kmw_loggers::logger_Statemachine_log_entry_t& logger) {
  for (auto it = logger.logger().cbegin(); it != logger.logger().cend();++it)
  {
   std::cout << (*it).states() << std::endl;
  }
 }
}}/*End section "Log Printers"*/
void test_1(){
 log4kmw_loggers::logger_Statemachine_log_entry.logger().init(log4kmw::persistence::memory_mapped_file("test.bin", 1024,false/* true*/));
 {
  using namespace log4kmw_test::meta_info;  using namespace log4kmw_loggers;
  using namespace log4kmw_states;
  /*log4kmw::get_value<0>(Current_states).set(1);
  log4kmw_loggers::log_event(log4kmw_events::Step(), log4kmw_loggers::logger_Statemachine_log_entry);
  log4kmw::get_value<0>(Current_states).set(2);
  log4kmw_loggers::log_event(log4kmw_events::Step(), log4kmw_loggers::logger_Statemachine_log_entry);
  log4kmw::get_value<0>(Current_states).set(3);
  log4kmw_loggers::log_event(log4kmw_events::Step(), log4kmw_loggers::logger_Statemachine_log_entry);
  log4kmw::get_value<0>(Current_states).reset(3);
  log4kmw_loggers::log_event(log4kmw_events::Step(), log4kmw_loggers::logger_Statemachine_log_entry);*/
  log_print(logger_Statemachine_log_entry);
 }
}
void run_all_tests(){
 test_1();
}
