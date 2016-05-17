/* LOG4KMW_RECORDS.CPP 
   CREATED Tue May 17 19:59:09 2016

   GENERATED BY logspec2cpp VERSION 0.4 (c) Krauss-Maffei-Wegmann GmbH & Co. KG. 
   BASED ON CEPS VERSION 1.1 (May 17 2016) BUILT WITH GCC 5.2.1 20151010 on GNU/LINUX 64BIT (C) BY THE AUTHORS OF ceps (ceps is hosted at github: https://github.com/cepsdev/ceps.git) 
   THIS IS A GENERATED FILE. DO NOT MODIFY.
*/



#include "log4kmw_records.hpp"

void log4kmw_records::make_record_from_states(Statemachine_log_entry& rec)
{
 std::unique_lock<std::mutex> lock(log4kmw_states::global_lock);
 rec.record() = log4kmw_records::Statemachine_log_entry_t(
   log4kmw_states::read_state_Synch_counter_no_lock(),
   log4kmw_states::read_state_Current_event_no_lock(),
   log4kmw_states::read_state_Current_states_no_lock()
 );
}
