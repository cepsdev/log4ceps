/* LOG4KMW_STATES.HPP 
   CREATED Tue Mar 29 10:36:29 2016

   GENERATED BY logspec2cpp VERSION 0.3 (c) Krauss-Maffei-Wegmann GmbH & Co. KG. 
   THIS IS A GENERATED FILE. DO NOT MODIFY.
*/


#ifndef LOG4KMW_STATES_HPP_INC
#define LOG4KMW_STATES_HPP_INC

#include "log4kmw_state.hpp"
#include<string>
#include<thread>
#include<mutex>

namespace log4kmw_states{
 extern std::mutex global_lock;
 extern log4kmw::State<SI::Quantity<SI::Unit<1,0,-1,0,0,0,0> ,double> > CGW_FzGeschwindigkeit;
 extern log4kmw::State<SI::Quantity<SI::Unit<1,0,0,0,0,0,0> ,int> > CMK_Gesamtkilometer;
 extern log4kmw::State<int, double, std::string> Monitor_status;
 extern log4kmw::State<std::string> Textfeld;
 extern log4kmw::State<std::string> a_string;
 extern log4kmw::State<int> an_int;

 typedef log4kmw::State<SI::Quantity<SI::Unit<1,0,-1,0,0,0,0> ,double> > CGW_FzGeschwindigkeit_t;
 typedef log4kmw::State<SI::Quantity<SI::Unit<1,0,0,0,0,0,0> ,int> > CMK_Gesamtkilometer_t;
 typedef log4kmw::State<int, double, std::string> Monitor_status_t;
 typedef log4kmw::State<std::string> Textfeld_t;
 typedef log4kmw::State<std::string> a_string_t;
 typedef log4kmw::State<int> an_int_t;

 void write_state_CGW_FzGeschwindigkeit(CGW_FzGeschwindigkeit_t const &);
 CGW_FzGeschwindigkeit_t   read_state_CGW_FzGeschwindigkeit(); 
 CGW_FzGeschwindigkeit_t   read_state_CGW_FzGeschwindigkeit_no_lock(); 
 void write_state_CMK_Gesamtkilometer(CMK_Gesamtkilometer_t const &);
 CMK_Gesamtkilometer_t   read_state_CMK_Gesamtkilometer(); 
 CMK_Gesamtkilometer_t   read_state_CMK_Gesamtkilometer_no_lock(); 
 void write_state_Monitor_status(Monitor_status_t const &);
 Monitor_status_t   read_state_Monitor_status(); 
 Monitor_status_t   read_state_Monitor_status_no_lock(); 
 void write_state_Textfeld(Textfeld_t const &);
 Textfeld_t   read_state_Textfeld(); 
 Textfeld_t   read_state_Textfeld_no_lock(); 
 void write_state_a_string(a_string_t const &);
 a_string_t   read_state_a_string(); 
 a_string_t   read_state_a_string_no_lock(); 
 void write_state_an_int(an_int_t const &);
 an_int_t   read_state_an_int(); 
 an_int_t   read_state_an_int_no_lock(); 
}

#endif
