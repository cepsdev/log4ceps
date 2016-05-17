/* LOG4KMW_RECORDS.HPP 
   CREATED Tue May 17 19:59:09 2016

   GENERATED BY logspec2cpp VERSION 0.4 (c) Krauss-Maffei-Wegmann GmbH & Co. KG. 
   BASED ON CEPS VERSION 1.1 (May 17 2016) BUILT WITH GCC 5.2.1 20151010 on GNU/LINUX 64BIT (C) BY THE AUTHORS OF ceps (ceps is hosted at github: https://github.com/cepsdev/ceps.git) 
   THIS IS A GENERATED FILE. DO NOT MODIFY.
*/


#ifndef LOG4KMW_RECORDS_HPP_INC
#define LOG4KMW_RECORDS_HPP_INC

#include "log4kmw_state.hpp"
#include "log4kmw_states.hpp"
#include "log4kmw_events.hpp"
#include "log4kmw_record.hpp"
#include<string>

namespace log4kmw_records{
 using Statemachine_log_entry_t = log4kmw::State_record <
   log4kmw_states::Synch_counter_t,
   log4kmw_states::Current_event_t,
   log4kmw_states::Current_states_t
 >;
struct Statemachine_log_entry{Statemachine_log_entry_t rec;Statemachine_log_entry_t& record(){return rec;} };

 void make_record_from_states(Statemachine_log_entry& rec);

}
#endif