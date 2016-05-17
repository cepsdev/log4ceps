/*
Copyright 2015, Krauss-Maffei-Wegmann GmbH & Co. KG.
All rights reserved.

Description:
 Event class.
*/


#ifndef INC_LOG4KMW_EVENT
#define INC_LOG4KMW_EVENT

#include <string>

namespace log4kmw {
	template<int ctr> class Event
	{
	public:
		enum { id_ = ctr };
		static const std::string description_;

		static int id() { return id_; }
		static std::string const & description() { return description_; }
	};

	template<typename T> void trigger_event(T const &) {}
}

#endif
