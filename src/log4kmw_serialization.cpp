/*
Copyright 2015, Krauss-Maffei-Wegmann GmbH & Co. KG.
All rights reserved.

@author Tomas Prerovsky <tomas.prerovsky@gmail.com>

Description:
States.
*/


#include "log4kmw_serialization.hpp"
#include <string>
#include <iostream>

/*Specialization for strings*/
size_t log4kmw::serialize_value(const std::string & v, char * buffer, size_t max_buffer_size, bool write_data, throw_exception_policy)
{
	auto n = v.length() + 1;
	if (!write_data) return n;
	if (n > max_buffer_size)
		throw state_serialization_error("Buffer overflow.");
	memcpy(buffer, v.c_str(), n);
	return n;
}

size_t log4kmw::serialize_value(const std::string & v, char * buffer, size_t max_buffer_size, bool write_data, nothrow_exception_policy)
{
	auto n = v.length() + 1;
	if (!write_data) return n;
	if (n > max_buffer_size) return 0;
	memcpy(buffer, v.c_str(), n);
	return n;
}

/*Deserialization: Specialization for strings*/

size_t log4kmw::deserialize_value(std::string & v, char * buffer, size_t max_buffer_size)
{
	auto n = strlen(buffer) + 1;

	if (n > max_buffer_size)
		throw state_serialization_error("Buffer overflow.");
	v = std::string(buffer);

	return n;
}


size_t log4kmw::serialize_value(	const log4kmw::Dynamic_bitset & v,
						char * buffer,
						size_t max_buffer_size,
						bool write_data,
						nothrow_exception_policy)
{

	size_t n = 0;
	n += sizeof(size_t) + v.allocated_memory_size();
	size_t len = v.size();
	if (max_buffer_size < n) return 0;
	if (!write_data) return n;


	*(size_t*)buffer = len;
	buffer += sizeof(size_t);
	memcpy(buffer,v.data(),len);
	return n;
}

size_t log4kmw::serialize_value(	const log4kmw::Dynamic_bitset & v,
						char * buffer,
						size_t max_buffer_size,
						bool write_data,
						throw_exception_policy)
{

	size_t n = 0;
	n += sizeof(size_t) + sizeof(size_t) + v.allocated_memory_size();
	if (!write_data) return n;
	if (max_buffer_size < n) throw state_serialization_error("Buffer overflow.");
	*(size_t*)buffer = v.allocated_memory_size();
	buffer += sizeof(size_t);
	*(size_t*)buffer = v.size();
	buffer += sizeof(size_t);
	memcpy(buffer,v.data(),v.allocated_memory_size());
	return n;
}


size_t log4kmw::deserialize_value(log4kmw::Dynamic_bitset & v, char * buffer, size_t max_buffer_size)
	{
		if (sizeof(size_t) > max_buffer_size)
			throw state_serialization_error("Buffer overflow.");
		size_t len = *(size_t*)buffer;
		buffer += sizeof(size_t);
		size_t bits = *(size_t*)buffer;
		buffer += sizeof(size_t);
		v = log4kmw::Dynamic_bitset(bits,buffer);
		return len + sizeof(size_t) + sizeof(size_t);
	}





