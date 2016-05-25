/*
Copyright 2015, Krauss-Maffei-Wegmann GmbH & Co. KG.
All rights reserved.

@author Tomas Prerovsky <tomas.prerovsky@gmail.com>

Description:
 Serialization routines.
*/


#ifndef INC_LOG4KMW_SERIALIZATION_HPP
#define INC_LOG4KMW_SERIALIZATION_HPP

#include "log4kmw_si_units.hpp"
#include "log4kmw_dynamic_bitset.hpp"
#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace log4kmw {

	/* Exceptions and exception policies. */
	class exception_policy {};
	class throw_exception_policy :public exception_policy  {};
	class nothrow_exception_policy :public exception_policy  {};

	class state_serialization_error : public std::runtime_error
	{
	public:
		state_serialization_error(const std::string & what) 
			: std::runtime_error(what)
		{}
	};

	/* General serialization */

	template<typename T, typename Exceptionpolicy>
	 size_t serialize_value(const T & v, char * buffer, size_t max_buffer_size, bool write_data, 
		                          Exceptionpolicy );

	 /* General serialization (throwing exceptions) */

	template<typename T> size_t serialize_value(const T & v, char * buffer, size_t max_buffer_size, bool write_data, throw_exception_policy)
	{
		if (!write_data) return sizeof(T);
		size_t v_size;
		if ((v_size = sizeof(T))  > max_buffer_size)
			throw state_serialization_error("Buffer overflow.");
		memcpy(buffer, &v, v_size);
		return v_size;
	}
	
	/* General serialization(no throw policy) */

	template<typename T> size_t serialize_value(const T & v, char * buffer, size_t max_buffer_size, bool write_data, nothrow_exception_policy)
	{
		if (!write_data) return sizeof(T);
		size_t v_size;
		if ((v_size = sizeof(T)) > max_buffer_size)
			return 0;
		memcpy(buffer, &v, v_size);
		return v_size;
	}

	/*Specializations (Serialization)*/


	/*Specialization for quantities (values with SI units)*/

	template<typename U, typename Scalar>
	size_t serialize_value(const SI::Quantity<U, Scalar>  & v, char * buffer, size_t max_buffer_size, bool write_data)
	{
		return serialize_value(v.value_, buffer, max_buffer_size, write_data);
	}

	/*Specialization for strings*/
	size_t serialize_value(const std::string & v, char * buffer, size_t max_buffer_size, bool write_data, throw_exception_policy);
	size_t serialize_value(const std::string & v, char * buffer, size_t max_buffer_size, bool write_data, nothrow_exception_policy);

	/*Specialization for vectors*/

	template<typename T>
	size_t serialize_value(	const std::vector<T> & v, 
							char * buffer, 
							size_t max_buffer_size, 
							bool write_data, 
							nothrow_exception_policy)
	{
		size_t n = 0;
		n += sizeof(size_t);
		size_t n_of_elems = v.size();
		for (size_t i = 0; i < n_of_elems; ++i)
		{
			if (max_buffer_size < n) return 0;
			n += serialize_value(v[i], buffer, max_buffer_size, false, nothrow_exception_policy());
		}//for
		if (!write_data) return n;
		
		*(size_t*)buffer = n_of_elems;
		buffer += sizeof(size_t);
		
		for (size_t i = 0; i < v.size(); ++i)
		{
			buffer += serialize_value(v[i], buffer, max_buffer_size, true, nothrow_exception_policy());
		}//for
		return n;
	}

	/*Specialization for dynamic bitsets*/

	size_t serialize_value(	const Dynamic_bitset & v,
							char * buffer,
							size_t max_buffer_size,
							bool write_data,
							nothrow_exception_policy);
	size_t serialize_value(	const Dynamic_bitset & v,
							char * buffer,
							size_t max_buffer_size,
							bool write_data,
							throw_exception_policy);
	/*Deserialization*/


	/*Deserialization: General templates*/

	template<typename T> size_t deserialize_value(T & v, char * buffer, size_t max_buffer_size)
	{
		if (sizeof(T) > max_buffer_size)
			throw state_serialization_error("Buffer overflow.");
		memcpy((char *)& v, buffer, sizeof(T));
		return sizeof(T);
	}

	/*Deserialization: Specialization for quantities*/
	
	template<typename U, typename Scalar>
	size_t deserialize_value(SI::Quantity<U, Scalar>  & v, char * buffer, size_t max_buffer_size)
	{
		return deserialize_value(v.value_, buffer, max_buffer_size);
	}

	/*Deserialization: Specialization for strings*/
	
	size_t deserialize_value(std::string & v, char * buffer, size_t max_buffer_size);
	/*Specialization for vectors*/

	template<typename T>
	size_t deserialize_value(std::vector<T> & v, char * buffer, size_t max_buffer_size)
	{
		if (sizeof(size_t) > max_buffer_size)
			throw state_serialization_error("Buffer overflow.");
		size_t n_of_elems = *(size_t*)buffer;
		buffer += sizeof(size_t);

		size_t n = sizeof(size_t);

	
		v.clear();
		for (size_t i = 0; i < n_of_elems; ++i)
		{
			T vv;			
			auto w = deserialize_value(vv, buffer, max_buffer_size-n);
			v.push_back(vv);
			n += w;
			if (max_buffer_size < n) 
				throw state_serialization_error("Buffer overflow.");
			buffer += w;			
		}//for
		
		return n;
	}

	/*Specialization for dynamic bitsets*/


	size_t deserialize_value(Dynamic_bitset & v, char * buffer, size_t max_buffer_size);

}//namespace log4kmw

#endif
