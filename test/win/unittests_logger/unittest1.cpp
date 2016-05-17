#include "stdafx.h"
#include "CppUnitTest.h"
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


using namespace log4kmw;
//using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace unittests_logger
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(SimpleLoggerExample)
		{
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

			Logger<Logbookentry, persistence::memory_mapped_file>
				logger{ persistence::memory_mapped_file("small_logbook.bin", 512, true) };
			
			logger.append(
				Logbookentry(
				Errortext(std::string("#1 Failure of device xyz. Errcode: 067889200jhgwz.")),
				Eventid(15),
				Currentspeed(10.0),
				Door_is_open(false))
				);
			logger.append(
				Logbookentry(
				Errortext(std::string("#2 Air conditioning failed. Errcode: 06788920hgjhggg0jhgwz.")),
				Eventid(12),
				Currentspeed(16.0),
				Door_is_open(false))
				);
			logger.append(
				Logbookentry(
				Errortext(std::string("#3 Door open. Errcode: 06780hg0---hggg0jhgwz.")),
				Eventid(112),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logger.append(
				Logbookentry(
				Errortext(std::string("#4 Temperature rising. Errcode: 06780hg0---h0jhgwz.")),
				Eventid(112),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logger.append(
				Logbookentry(
				Errortext(std::string("#5 Man jumped out. Errcode: 06780hg0---h0jhgwz.")),
				Eventid(1),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logger.append(
				Logbookentry(
				Errortext(std::string("#6 Man jumped out. Errcode: 06780hg0---h0jhgwz.")),
				Eventid(1),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logger.append(
				Logbookentry(
				Errortext(std::string("#7 Unmanned vehicle. Errcode: 780hg0---h0jhgwz.")),
				Eventid(1),
				Currentspeed(18.0),
				Door_is_open(true))
				);
			logger.append(
				Logbookentry(
				Errortext(std::string("#8 Crash. 780hg0---h0jhgwz.")),
				Eventid(2),
				Currentspeed(0),
				Door_is_open(true))
				);
			logger.append(
				Logbookentry(
				Errortext(std::string("#9 Fire. FFFFFF.")),
				Eventid(3),
				Currentspeed(0),
				Door_is_open(true))
				);

			{
				/*EXPECTATION

				We inserted 9 log entries. The logfile's size is 512 (including the header). Therefore
				after inserting the ninth entry the very first entry is overwritten.
				If we loop through the entries via the common pattern/idiom using iterators the very first
				entry should be entry #2.
				*/
				using namespace Microsoft::VisualStudio::CppUnitTestFramework;
				int counter = 0;
				for (
					auto it = logger.cbegin(); // get iterator pointing to the very first entry in the logbook
					it != logger.cend();       // loop while we haven't reached the end
					++it,++counter			   // get next element (++it)
					)
				{
					auto r = *it;   // r contains the entry the iterator it is pointing to
					if (counter == 0) // counter == 0 <=> r contains the very first entry in the logbook
					{
						//Check values
						Assert::IsTrue(get_value<0>(r) ==
							Errortext(std::string("#2 Air conditioning failed. Errcode: 06788920hgjhggg0jhgwz.")));
						Assert::IsTrue(get_value<1>(r) == Eventid(12));
						Assert::IsTrue(get_value<2>(r) == Currentspeed(16.0));
						Assert::IsTrue(get_value<3>(r) == Door_is_open(false));
					}

				}
			}

		}

	};
}