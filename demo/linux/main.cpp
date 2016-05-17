/*
Copyright 2015, Krauss-Maffei-Wegmann GmbH & Co. KG.
All rights reserved.

@author Tomas Prerovsky <tomas.prerovsky@gmail.com>


Description:

Small example application using the log4kmw - API in combination with the logspec2cpp tooling.

The main thread updates the "Textfeld" state (see the log specification) by asking the user for text input.
Meanwhile a second thread simulates a monitor by triggering MonitorAn/MonitorAus events.
By typing "quit" the application terminates and prints the current logbook entries.

IMPORTANT: Please make sure you have a current version of logspec2cpp.exe located in $(PROJECT)\..\..\bin\Release

PROJECT STRUCTURE:

$(PROJECT_PATH)
  /generated_files     ==> logspec2cpp.exe writes output to this directory
  /log_spec            ==> The log specification (the input for logspec2cpp)
  /Debug               ==> exe with debug information
  /Release             ==> optimized exe (without debug symbols)

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

/*The following includes refer to generated headers.
  Please refer to the log specification located in $PROJECT/log_spec/general.def.
*/
#include "log4kmw_states.hpp"
#include "log4kmw_records.hpp"
#include "log4kmw_loggers.hpp"


using namespace log4kmw;


const auto logbook_size = 2048;// total size of file where the logbook data is written to

bool exit_request = false;//control variable for threads

void monitor_thread_func();//thread simulating monitor




int main(int argc, char* argv[])
{
	using namespace std;
	using namespace log4kmw;
	using namespace log4kmw_loggers;
	using namespace log4kmw_events;

	logger_status_all.init(log4kmw::persistence::memory_mapped_file("demo_logbook.bin", logbook_size, true));
	std::thread monitor_thread{ monitor_thread_func };//starts monitor thread
	try
	{
		for (;;)
		{
			std::string buffer;
			std::cout << "USER INPUT>>"; 			
			std::getline(std::cin,buffer);
			
			log4kmw_states::write_state_Textfeld(buffer);//update state
			
			//log4kmw_loggers::log_event(log4kmw_events::BenuzerEingabe(), log4kmw_loggers::logger_status_all);
			if (buffer == "quit") break;
		}//for
		exit_request = true;//Signaling shutdown request.
		std::cout << "\n***Going down...";
	}
	catch (std::runtime_error const & re)
	{
		std::cout << "*** " << re.what() << std::endl;
	}
	monitor_thread.join();//wait for the monitor thread to terminate
	std::cout << "\nLogentries ():\n";

	/**Loop through logbook entries and print.*/
	for (
		auto it = log4kmw_loggers::logger_status_all.cbegin(); // get iterator pointing to the very first entry in the logbook
		it != log4kmw_loggers::logger_status_all.cend();       // loop while we haven't reached the end
		++it		   
		)
	{
		//auto r = *it;   // r contains the entry the iterator it is pointing to
		std::cout << (*it).states() << std::endl;
	}
	
	std::cout << "Please press ENTER.\n";
	char ch;
	std::cin.get(ch);
	std::cout << "Bye!\n";
}

void monitor_thread_func()
{
	using namespace log4kmw;

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(10, 2000);
	for (; !exit_request;)
	{
		std::chrono::milliseconds delay{uni(rng)};
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		auto m = log4kmw_states::read_state_Monitor_status();
		get_value<0>(m) = !((bool)get_value<0>(m));
		log4kmw_states::write_state_Monitor_status(m);
		if (get_value<0>(m)) log4kmw_loggers::log_event(log4kmw_events::MonitorAn(), log4kmw_loggers::logger_status_all);
		else log4kmw_loggers::log_event(log4kmw_events::MonitorAus(), log4kmw_loggers::logger_status_all);
	}
}


