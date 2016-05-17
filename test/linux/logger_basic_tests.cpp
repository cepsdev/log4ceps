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

#include "log4kmw_serialization.hpp"
#include "log4kmw_state.hpp"
#include "log4kmw_record.hpp"
#include "log4kmw_ringbuffer.hpp"
#include "log4kmw_logger.hpp"

#include "gtest/gtest.h"


using namespace log4kmw;




TEST(LoggerTest, CompositeStatesWithOverrun) {


			using Errortext = State < std::string >;
			using Eventid = State < int >;
			using Currentspeed = State < SI::Quantity<SI::MpS, double> >;
			using Door_is_open = State < bool >;

			using Logbookentry = State_record <
				Errortext,
				Eventid,
				Currentspeed,
				Door_is_open
			>;

			Logbookentry e;
			std::vector<Logbookentry> logbook_entries_as_appended;

			Logger<Logbookentry, persistence::memory_mapped_file>
				logger{ persistence::memory_mapped_file("small_logbook.bin", 512, true) };


			logger.append(
				e = Logbookentry(
				Errortext(std::string("#1 Failure of device xyz. Errcode: 067889200jhgwz.")),
				Eventid(15),
				Currentspeed(10.0),
				Door_is_open(false))
				);
			logbook_entries_as_appended.push_back(e);

			logger.append(
				e = Logbookentry(
				Errortext(std::string("#2 Air conditioning failed. Errcode: 06788920hgjhggg0jhgwz.")),
				Eventid(12),
				Currentspeed(16.0),
				Door_is_open(false))
				);
			logbook_entries_as_appended.push_back(e);

			logger.append(
				e = Logbookentry(
				Errortext(std::string("#3 Door open. Errcode: 06780hg0---hggg0jhgwz.")),
				Eventid(112),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logbook_entries_as_appended.push_back(e);

			logger.append(
				e = Logbookentry(
				Errortext(std::string("#4 Temperature rising. Errcode: 06780hg0---h0jhgwz.")),
				Eventid(112),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logbook_entries_as_appended.push_back(e);

			logger.append(
				e = Logbookentry(
				Errortext(std::string("#5 Man jumped out. Errcode: 06780hg0---h0jhgwz.")),
				Eventid(1),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logbook_entries_as_appended.push_back(e);


			logger.append(
				e = Logbookentry(
				Errortext(std::string("#6 Man jumped out. Errcode: 06780hg0---h0jhgwz.")),
				Eventid(1),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logbook_entries_as_appended.push_back(e);

			logger.append(
				e = Logbookentry(
				Errortext(std::string("#7 Unmanned vehicle. Errcode: 780hg0---h0jhgwz.")),
				Eventid(1),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logbook_entries_as_appended.push_back(e);

			logger.append(
				e = Logbookentry(
				Errortext(std::string("#8 Crash. 780hg0---h0jhgwz.")),
				Eventid(2),
				Currentspeed(0),
				Door_is_open(true))
				);
			logbook_entries_as_appended.push_back(e);

			logger.append(
				e = Logbookentry(
				Errortext(std::string("#9 Fire. FFFFFF.")),
				Eventid(3),
				Currentspeed(0),
				Door_is_open(true))
				);
			logbook_entries_as_appended.push_back(e);



			{
				/*EXPECTATION

				We inserted 9 log entries. The logfile's size is 512 (including the header). Therefore
				after inserting the ninth entry the very first entry is overwritten.
				If we loop through the entries via the common pattern/idiom using iterators the very first
				entry should be entry #2.
				*/

				std::vector<Logbookentry> logbook_entries_as_stored;

				for (
					auto it = logger.cbegin(); // get iterator pointing to the very first entry in the logbook
					it != logger.cend();       // loop while we haven't reached the end
					++it			   // get next element (++it)
					)
				{
					auto r = *it;   // r contains the entry the iterator it is pointing to
					logbook_entries_as_stored.push_back(r);
					//std::cout << (State<Errortext,Eventid,Currentspeed,Door_is_open> )r << std::endl;
				}

				EXPECT_TRUE(logbook_entries_as_stored.size() > 0);
				EXPECT_TRUE(logbook_entries_as_stored.size() <= logbook_entries_as_appended.size());
				auto diff = logbook_entries_as_appended.size()-logbook_entries_as_stored.size();
				EXPECT_TRUE(diff > 0);

				for(int j = logbook_entries_as_stored.size()-1;j>=0;--j)
				{
				  EXPECT_TRUE(logbook_entries_as_stored[j] == logbook_entries_as_appended[j+diff]);
				}
			}
}
