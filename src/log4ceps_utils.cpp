#include "log4ceps_state.hpp"
#include <string>

std::ostream & operator << (std::ostream & os, timespec const & ts){
	auto r = localtime(&ts.tv_sec);
	char result[256] = {0};

	static char wday_name[7][4] = {
	        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	static char mon_name[12][4] = {
	        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	sprintf(result, "\"%.3s %.3s%3d %.2d:%.2d:%.2d.%ld %d\"",
	        wday_name[r->tm_wday],
	        mon_name[r->tm_mon],
	        r->tm_mday, r->tm_hour,
	        r->tm_min, r->tm_sec,
			ts.tv_nsec,
	        1900 + r->tm_year);
	os << result;
	return os;
}
