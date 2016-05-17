/* LOG4KMW_LOGGERS.HPP 
   CREATED Tue Mar 29 10:36:29 2016

   GENERATED BY logspec2cpp VERSION 0.3 (c) Krauss-Maffei-Wegmann GmbH & Co. KG. 
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
 using logger_abc_t = log4kmw::Logger<log4kmw_records::abc, log4kmw::persistence::memory_mapped_file>;
 extern log4kmw::Logger<log4kmw_records::abc, log4kmw::persistence::memory_mapped_file> logger_abc;
 extern std::mutex logger_mutex_abc;
 using logger_def_t = log4kmw::Logger<log4kmw_records::def, log4kmw::persistence::memory_mapped_file>;
 extern log4kmw::Logger<log4kmw_records::def, log4kmw::persistence::memory_mapped_file> logger_def;
 extern std::mutex logger_mutex_def;
 using logger_status_all_t = log4kmw::Logger<log4kmw_records::status_all, log4kmw::persistence::memory_mapped_file>;
 extern log4kmw::Logger<log4kmw_records::status_all, log4kmw::persistence::memory_mapped_file> logger_status_all;
 extern std::mutex logger_mutex_status_all;
 using logger_record2_t = log4kmw::Logger<log4kmw_records::record2, log4kmw::persistence::memory_mapped_file>;
 extern log4kmw::Logger<log4kmw_records::record2, log4kmw::persistence::memory_mapped_file> logger_record2;
 extern std::mutex logger_mutex_record2;


 void log_event(log4kmw_events::BenuzerEingabe const &, logger_abc_t &);
 void log_event(log4kmw_events::BenuzerEingabe const &, logger_def_t &);
 void log_event(log4kmw_events::BenuzerEingabe const &, logger_status_all_t &);
 void log_event(log4kmw_events::BenuzerEingabe const &, logger_record2_t &);
 void log_event(log4kmw_events::MonitorAn const &, logger_abc_t &);
 void log_event(log4kmw_events::MonitorAn const &, logger_def_t &);
 void log_event(log4kmw_events::MonitorAn const &, logger_status_all_t &);
 void log_event(log4kmw_events::MonitorAn const &, logger_record2_t &);
 void log_event(log4kmw_events::MonitorAus const &, logger_abc_t &);
 void log_event(log4kmw_events::MonitorAus const &, logger_def_t &);
 void log_event(log4kmw_events::MonitorAus const &, logger_status_all_t &);
 void log_event(log4kmw_events::MonitorAus const &, logger_record2_t &);
 void log_event(log4kmw_events::NullstellungLenkwinkel const &, logger_abc_t &);
 void log_event(log4kmw_events::NullstellungLenkwinkel const &, logger_def_t &);
 void log_event(log4kmw_events::NullstellungLenkwinkel const &, logger_status_all_t &);
 void log_event(log4kmw_events::NullstellungLenkwinkel const &, logger_record2_t &);
 void log_event(log4kmw_events::ResetBetriebsstundenGasfilter const &, logger_abc_t &);
 void log_event(log4kmw_events::ResetBetriebsstundenGasfilter const &, logger_def_t &);
 void log_event(log4kmw_events::ResetBetriebsstundenGasfilter const &, logger_status_all_t &);
 void log_event(log4kmw_events::ResetBetriebsstundenGasfilter const &, logger_record2_t &);
}//namespace log4kmw_loggers
#endif