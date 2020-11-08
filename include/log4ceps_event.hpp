#ifndef INC_LOG4CEPS_EVENT
#define INC_LOG4CEPS_EVENT

#include <string>

namespace log4ceps {
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
