/* LOG4KMW_STATES.CPP 
   CREATED Tue Mar 29 10:36:29 2016

   GENERATED BY logspec2cpp VERSION 0.3 (c) Krauss-Maffei-Wegmann GmbH & Co. KG. 
   THIS IS A GENERATED FILE. DO NOT MODIFY.
*/


#include "log4kmw_states.hpp"


std::mutex log4kmw_states::global_lock;
log4kmw::State<SI::Quantity<SI::Unit<1,0,-1,0,0,0,0> ,double> > log4kmw_states::CGW_FzGeschwindigkeit(0);
log4kmw::State<SI::Quantity<SI::Unit<1,0,0,0,0,0,0> ,int> > log4kmw_states::CMK_Gesamtkilometer(1000);
log4kmw::State<int, double, std::string> log4kmw_states::Monitor_status(0, 0.5, "ok");
log4kmw::State<std::string> log4kmw_states::Textfeld("efgh");
log4kmw::State<std::string> log4kmw_states::a_string("abc");
log4kmw::State<int> log4kmw_states::an_int(1);

void log4kmw_states::write_state_CGW_FzGeschwindigkeit( log4kmw_states::CGW_FzGeschwindigkeit_t const & v)
{
  std::unique_lock<std::mutex> lock(global_lock);
  CGW_FzGeschwindigkeit = v;
}

log4kmw_states::CGW_FzGeschwindigkeit_t   log4kmw_states::read_state_CGW_FzGeschwindigkeit()
{
  std::unique_lock<std::mutex> lock(global_lock);
  return log4kmw_states::CGW_FzGeschwindigkeit;
}
log4kmw_states::CGW_FzGeschwindigkeit_t   log4kmw_states::read_state_CGW_FzGeschwindigkeit_no_lock()
{
  return log4kmw_states::CGW_FzGeschwindigkeit;
}
void log4kmw_states::write_state_CMK_Gesamtkilometer( log4kmw_states::CMK_Gesamtkilometer_t const & v)
{
  std::unique_lock<std::mutex> lock(global_lock);
  CMK_Gesamtkilometer = v;
}

log4kmw_states::CMK_Gesamtkilometer_t   log4kmw_states::read_state_CMK_Gesamtkilometer()
{
  std::unique_lock<std::mutex> lock(global_lock);
  return log4kmw_states::CMK_Gesamtkilometer;
}
log4kmw_states::CMK_Gesamtkilometer_t   log4kmw_states::read_state_CMK_Gesamtkilometer_no_lock()
{
  return log4kmw_states::CMK_Gesamtkilometer;
}
void log4kmw_states::write_state_Monitor_status( log4kmw_states::Monitor_status_t const & v)
{
  std::unique_lock<std::mutex> lock(global_lock);
  Monitor_status = v;
}

log4kmw_states::Monitor_status_t   log4kmw_states::read_state_Monitor_status()
{
  std::unique_lock<std::mutex> lock(global_lock);
  return log4kmw_states::Monitor_status;
}
log4kmw_states::Monitor_status_t   log4kmw_states::read_state_Monitor_status_no_lock()
{
  return log4kmw_states::Monitor_status;
}
void log4kmw_states::write_state_Textfeld( log4kmw_states::Textfeld_t const & v)
{
  std::unique_lock<std::mutex> lock(global_lock);
  Textfeld = v;
}

log4kmw_states::Textfeld_t   log4kmw_states::read_state_Textfeld()
{
  std::unique_lock<std::mutex> lock(global_lock);
  return log4kmw_states::Textfeld;
}
log4kmw_states::Textfeld_t   log4kmw_states::read_state_Textfeld_no_lock()
{
  return log4kmw_states::Textfeld;
}
void log4kmw_states::write_state_a_string( log4kmw_states::a_string_t const & v)
{
  std::unique_lock<std::mutex> lock(global_lock);
  a_string = v;
}

log4kmw_states::a_string_t   log4kmw_states::read_state_a_string()
{
  std::unique_lock<std::mutex> lock(global_lock);
  return log4kmw_states::a_string;
}
log4kmw_states::a_string_t   log4kmw_states::read_state_a_string_no_lock()
{
  return log4kmw_states::a_string;
}
void log4kmw_states::write_state_an_int( log4kmw_states::an_int_t const & v)
{
  std::unique_lock<std::mutex> lock(global_lock);
  an_int = v;
}

log4kmw_states::an_int_t   log4kmw_states::read_state_an_int()
{
  std::unique_lock<std::mutex> lock(global_lock);
  return log4kmw_states::an_int;
}
log4kmw_states::an_int_t   log4kmw_states::read_state_an_int_no_lock()
{
  return log4kmw_states::an_int;
}