/*
Copyright 2015, Krauss-Maffei-Wegmann GmbH & Co. KG.
All rights reserved.

@author Tomas Prerovsky <tomas.prerovsky@gmail.com>

Description:
Records.
*/


#ifndef INC_LOG4KMW_RECORD_HPP
#define INC_LOG4KMW_RECORD_HPP

#include "log4kmw_si_units.hpp"
#include "log4kmw_serialization.hpp"
#include "log4kmw_state.hpp"
#include <iostream>


namespace log4kmw {


	template<typename... Ts>
	class State_record: public State<Ts...>
	{

	public:
		State_record( Ts... v) : State<Ts...>(v...) {}
		State_record() = default;
		State<Ts...> const & states() const { return *this; }
		State<Ts...> & states()  { return *this; }
	};

	template <int N, typename... Ts>
	typename Type_of_Nth_element<N, Ts...>::type & get_value(State_record< Ts...>& r)
	{
		return get_value<N, Ts...>(r.states());
	}


	template<typename... Ts>
	size_t serialize_rec_helper(State<Ts...> const & s, char * buffer, size_t size, bool write_data);


	template<typename Head, typename... Ts>
	size_t serialize_rec_helper(State<Head, Ts...> const & s, char * buffer, size_t size, bool write_data)
	{
		auto t = s.head().serialize(buffer, size, write_data, log4kmw::throw_exception_policy()); 
		return t + serialize_rec_helper<Ts...>(s.inherited(), buffer + t, size - t, write_data);
	}

	template<typename... Ts>
	size_t serialize_rec_helper(State<Ts...> const & , char *, size_t, bool)
	{
		return 0;
	}

	size_t serialize_rec_helper(State<> const &, char * buffer, size_t size, bool write_data);

	
	template<typename Head, typename... Ts>
	size_t serialize(State_record< Head, Ts...> const & rec, char * buffer, size_t size, bool write_data = true)
	{
		return serialize_rec_helper(rec.states(), buffer, size, write_data);
	}


	template<typename Head, typename T, typename... Ts>
	size_t deserialize_rec_helper(State<Head, T, Ts...>  & s, char * buffer, size_t size)
	{
		auto t = s.head().deserialize(buffer, size);
		return t + deserialize_rec_helper<T, Ts...>(s.inherited(), buffer + t, size - t);
	}

	template<typename Head, typename... Ts>
	size_t deserialize_rec_helper(State<Head, Ts...>  & s, char * buffer, size_t size)
	{
		return s.head().deserialize(buffer, size);
	}

	template<typename... Ts>
	size_t deserialize(State_record< Ts...>  & rec, char * buffer, size_t size)
	{
		return deserialize_rec_helper(rec.states(), buffer, size);
	}
}//namespace log4kmw

#endif
