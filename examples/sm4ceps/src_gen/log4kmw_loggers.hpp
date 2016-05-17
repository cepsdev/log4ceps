/* LOG4KMW_LOGGERS.HPP 
   CREATED Tue May 17 19:59:09 2016

   GENERATED BY logspec2cpp VERSION 0.4 (c) Krauss-Maffei-Wegmann GmbH & Co. KG. 
   BASED ON CEPS VERSION 1.1 (May 17 2016) BUILT WITH GCC 5.2.1 20151010 on GNU/LINUX 64BIT (C) BY THE AUTHORS OF ceps (ceps is hosted at github: https://github.com/cepsdev/ceps.git) 
   THIS IS A GENERATED FILE. DO NOT MODIFY.
*/


#ifndef LOG4KMW_LOGGERS_HPP_INC
#define LOG4KMW_LOGGERS_HPP_INC

#include "log4kmw_state.hpp"
#include "log4kmw_states.hpp"
#include "log4kmw_events.hpp"
#include "log4kmw_record.hpp"
#include "log4kmw_logger.hpp"
#include "log4kmw_events.hpp"
#include<string>
#include <thread>
#include <mutex>

namespace log4kmw_loggers{
 struct logger_Statemachine_log_entry_t{log4kmw::Logger<log4kmw_records::Statemachine_log_entry_t, log4kmw::persistence::memory_mapped_file>log;
 log4kmw::Logger<log4kmw_records::Statemachine_log_entry_t, log4kmw::persistence::memory_mapped_file>& logger(){return log;}};
 extern logger_Statemachine_log_entry_t logger_Statemachine_log_entry;
 extern std::mutex logger_mutex_Statemachine_log_entry;


 void log_event(log4kmw_events::Step const &, logger_Statemachine_log_entry_t &);
}//namespace log4kmw_loggers
#endif
