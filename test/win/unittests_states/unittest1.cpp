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
#include "log4kmw_state.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittests_states
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(atomic_state_string)
		{
			// TODO: Your test code here
			log4kmw::State < std::string > a{ "abCdeF" };
			log4kmw::State < std::string > b;
			a = b;
			Assert::IsTrue(a == b);

			b = std::string{ "gHIj" };

			Assert::IsTrue(a != b);
			log4kmw::State < std::string > c;
			c = std::string("gHIj");
			Assert::IsTrue(b == c);
			log4kmw::State < std::string > d("gHIj");
			Assert::IsTrue(c == d);
			log4kmw::State < std::string > e(d);
			Assert::IsTrue(c == e);
		}
		TEST_METHOD(composite_state_string_int)
		{
			// TODO: Your test code here
			log4kmw::State < std::string,int > a( "abCdeF",0 );

			Assert::IsTrue(log4kmw::get_value<0>(a) == "abCdeF");
			Assert::IsTrue(log4kmw::get_value<1>(a) == 0);

			log4kmw::State < double, std::string, int > b(2.0,"klmnOP", 1);

			Assert::IsTrue(log4kmw::get_value<0>(b) == 2.0);
			Assert::IsTrue(log4kmw::get_value<1>(b) == "klmnOP");
			Assert::IsTrue(log4kmw::get_value<2>(b) == 1);

			log4kmw::State < double, std::string, int, SI::Quantity<SI::MpS> > c(3.1, "qRsTuvW", 10,20.65);


			Assert::IsTrue(log4kmw::get_value<0>(c) == 3.1);
			Assert::IsTrue(log4kmw::get_value<1>(c) == "qRsTuvW");
			Assert::IsTrue(log4kmw::get_value<2>(c) == 10 );
			Assert::IsTrue(log4kmw::get_value<3>(c) == SI::Quantity<SI::MpS>(20.65) );		
			
			log4kmw::State < double, std::string, int, SI::Quantity<SI::MpS> > d;
			
			d = c;

			Assert::IsTrue(log4kmw::get_value<0>(d) == 3.1);
			Assert::IsTrue(log4kmw::get_value<1>(d) == "qRsTuvW");
			Assert::IsTrue(log4kmw::get_value<2>(d) == 10);
			Assert::IsTrue(log4kmw::get_value<3>(d) == SI::Quantity<SI::MpS>(20.65));

			Assert::IsTrue(log4kmw::get_value<0>(c) == 3.1);
			Assert::IsTrue(log4kmw::get_value<1>(c) == "qRsTuvW");
			Assert::IsTrue(log4kmw::get_value<2>(c) == 10);
			Assert::IsTrue(log4kmw::get_value<3>(c) == SI::Quantity<SI::MpS>(20.65));

		}

	};
}