/* LOG4KMW_LOGGERS.CPP 
   CREATED Tue May 24 15:43:11 2016

   GENERATED BY logspec2cpp VERSION 0.5 (c) Krauss-Maffei-Wegmann GmbH & Co. KG. 
   BASED ON CEPS VERSION 1.1 (Apr 30 2016) BUILT WITH GCC 4.9.0 on GNU/LINUX 64BIT (C) BY THE AUTHORS OF ceps (ceps is hosted at github: https://github.com/cepsdev/ceps.git) 
   THIS IS A GENERATED FILE. DO NOT MODIFY.
*/



#include "log4kmw_records.hpp"
#include "log4kmw_loggers.hpp"

 log4kmw_loggers::logger_Statemachine_log_entry_t log4kmw_loggers::logger_Statemachine_log_entry;
std::mutex log4kmw_loggers::logger_mutex_Statemachine_log_entry;
 void log4kmw_loggers::log_event(log4kmw_events::Step const & ev, log4kmw_loggers::logger_Statemachine_log_entry_t & log)
{
 if(!log.logger()) throw std::runtime_error("Logger 'Statemachine_log_entry' invalid (not initialized?)");
 log4kmw_records::Statemachine_log_entry rec;
 log4kmw_records::make_record_from_states(rec);
 {
  std::unique_lock<std::mutex> lock(log4kmw_loggers::logger_mutex_Statemachine_log_entry);
  log.logger().append(rec.record());
 }
}

