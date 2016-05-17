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

using namespace log4kmw;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

const size_t buffer_size = 4096;

template<typename T>
size_t sizeof_representation(const T& v)
{
	return sizeof(v);
}

size_t sizeof_representation(const std::string& v)
{
	return v.length() + 1;
}

template<typename T>
void serialize_deserialize_and_compare(T& value, char* buffer, size_t buf_size)
{
	auto written = serialize_value(value, buffer, buf_size, true, log4kmw::nothrow_exception_policy());
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(written, sizeof_representation(value));
	T value_temp{};
	auto read = deserialize_value(value_temp, buffer, buf_size);
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(read, sizeof_representation(value_temp));
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(value, value_temp );
}

template<typename U,typename T>
void serialize_deserialize_and_compare(SI::Quantity<U,T>& value, char* buffer, size_t buf_size)
{
	auto written = serialize_value(value, buffer, buf_size, true, log4kmw::nothrow_exception_policy());
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(written, sizeof_representation(value));
	SI::Quantity<U, T> value_temp{};
	auto read = deserialize_value(value_temp, buffer, buf_size);
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(read, sizeof_representation(value_temp));
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(value ==  value_temp);
}

template<typename T>
void serialize_deserialize_and_compare(std::vector<T>& value, char* buffer, size_t buf_size)
{
	auto written = serialize_value(value, buffer, buf_size, true, log4kmw::nothrow_exception_policy());
	
	std::vector<T> value_temp{};
	auto read = deserialize_value(value_temp, buffer, buf_size);
	
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(value.size() == value_temp.size());
	for (size_t i = 0; i < value.size();++i)
		Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(value[i] == value_temp[i]);
}

namespace unittests_serialization
{		
	TEST_CLASS(UnitTest1)
	{
		
	public:		
		TEST_METHOD(serialization_of_builtin_and_standard_types)
		{
			char buffer[buffer_size] = { 0 };
			srand(time(nullptr));
			
			// serialization of int
			for (int i = 0; i < 4096; ++i)
			{
				int v = rand();
				serialize_deserialize_and_compare(v,buffer,buffer_size);

			}//for

			// serialization of bool
			for (int i = 0; i < 4096; ++i)
			{
				bool v = rand();
				serialize_deserialize_and_compare(v, buffer, buffer_size);

			}//for

			// serialization of short
			for (int i = 0; i < 4096; ++i)
			{
				short v = rand();
				serialize_deserialize_and_compare(v, buffer, buffer_size);

			}//for

			// serialization of double
			for (int i = 0; i < 4096; ++i)
			{
				double v = (double)rand() + (double)(rand() % 1000) / 1000.0;
				serialize_deserialize_and_compare(v, buffer, buffer_size);
			}//for

			// serialization of float
			for (int i = 0; i < 4096; ++i)
			{
				float v = (float)rand() + (float)(rand() % 1000) / 1000.0;
				serialize_deserialize_and_compare(v, buffer, buffer_size);
			}//for

			// serialization of char
			for (int i = 0; i < 4096; ++i)
			{
				char v = (char)rand();
				serialize_deserialize_and_compare(v, buffer, buffer_size);
			}//for

			// serialization of std::string
			for (int i = 0; i < 4096; ++i)
			{
				int n = rand() % 10;
				std::stringstream ss;
				for (int j = 0; j < n; ++j)
				{
					ss << rand() % 10;
				}//for
				std::string v{ss.str()};
				serialize_deserialize_and_compare(v, buffer, buffer_size);
			}//for

			// serialization of std::vector<int>
			for (int i = 0; i < 4096; ++i)
			{
				int n = rand() % 10;
				std::vector<int> v;
				for (int j = 0; j < n; ++j)
				{
					v.push_back(rand());
				}//for
				serialize_deserialize_and_compare(v, buffer, buffer_size);
			}//for

			// serialization of std::vector<std::string>
			for (int i = 0; i < 4096; ++i)
			{
				int n = rand() % 10;
				std::vector<std::string> v;
				for (int j = 0; j < n; ++j)
				{
					std::stringstream ss;
					int m = rand() % 32;
					for (int k = 0; k < m; ++k)
					{
						ss << rand() % 10;
					}//for
					v.push_back(ss.str());
				}//for
				serialize_deserialize_and_compare(v, buffer, buffer_size);
			}//for
		}

		TEST_METHOD(serialization_of_quantities)
		{
			char buffer[buffer_size] = { 0 };
			srand(time(nullptr));

			// serialization of int quantities
			for (int i = 0; i < 4096; ++i)
			{
				SI::Quantity<SI::Ampere,int> v = rand();
				serialize_deserialize_and_compare(v, buffer, buffer_size);

			}//for
			// serialization of bool quantities
			for (int i = 0; i < 4096; ++i)
			{
				SI::Quantity<SI::Ampere, bool> v = rand();
				serialize_deserialize_and_compare(v, buffer, buffer_size);

			}//for
			// serialization of double quantities
			for (int i = 0; i < 4096; ++i)
			{
				SI::Quantity<SI::Metre, double> v = (double)rand() + (double)(rand() % 1000) / 1000.0;
				serialize_deserialize_and_compare(v, buffer, buffer_size);
			}//for
			// serialization of float quantities
			for (int i = 0; i < 4096; ++i)
			{
				SI::Quantity<SI::Metre, float> v = (float)rand() + (float)(rand() % 1000) / 1000.0;
				serialize_deserialize_and_compare(v, buffer, buffer_size);
			}//for

			// serialization of std::vector<SI::Quantity<SI::Metre,double>>
			for (int i = 0; i < 4096; ++i)
			{
				int n = rand() % 10;
				std::vector<SI::Quantity<SI::Metre, double>> v;
				for (int j = 0; j < n; ++j)
				{
					v.push_back(rand());
				}//for
				serialize_deserialize_and_compare(v, buffer, buffer_size);
			}//for
		}
	};
}