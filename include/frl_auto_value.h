#ifndef FRL_AUTO_VALUE_H_
#define FRL_AUTO_VALUE_H_

#include "frl_string.h"

namespace frl
{
	template<typename TYPE>
	class ExtractPtr
	{
	public:
		typedef TYPE Type;
	};

	template<typename TYPE>
	class ExtractPtr<TYPE*>
	{
	public:
		typedef TYPE Type;
	};

	template< 
					typename TYPE, 
					TYPE initValue = 0, 
					TYPE defaultValue = initValue 
				> 
	class AutoValue
	{
		TYPE value;
	public:
		// Constructor
		AutoValue() : value(initValue) {}

		template<class T2>
		AutoValue(T2& t) : value(t) {}

		// Data
		const TYPE& Data() const { return value; }
		TYPE& Data() { return value; }

		// Operators
		// T
		operator const TYPE&() const { return value; }
		operator TYPE&() { return value; }

		// Functor
		TYPE& operator()() { return value; }
		const TYPE& operator()() const { return value; }

		// Arithmetic
		template<typename T2> const AutoValue& operator=(const T2& t) { value=t; return *this; }
		template<typename T2> const AutoValue& operator+=(const T2& t) { value+=t; return *this; }
		template<typename T2> const AutoValue& operator-=(const T2& t) { value-=t; return *this; }
		template<typename T2> const AutoValue& operator*=(const T2& t) { value*=t; return *this; }
		template<typename T2> const AutoValue& operator/=(const T2& t) { value/=t; return *this; }
		template<typename T2> const AutoValue& operator%=(const T2& t) { value%=t; return *this; }
		template<typename T2> const AutoValue& operator&=(const T2& t) { value&=t; return *this; }
		template<typename T2> const AutoValue& operator|=(const T2& t) { value|=t; return *this; }
		template<typename T2> const AutoValue& operator^=(const T2& t) { value^=t; return *this; }
		template<typename T2> const AutoValue& operator<<=(const T2& t) { value<<=t; return *this; }
		template<typename T2> const AutoValue& operator>>=(const T2& t) { value>>=t; return *this; }

		// Compare
		template<typename T2> const bool operator==(const T2& t) const { return value==t; }
		template<typename T2> const bool operator!=(const T2& t) const { return value!=t; }
		template<typename T2> const bool operator>(const T2& t) const { return value>t; }
		template<typename T2> const bool operator>=(const T2& t) const { return value>=t; }
		template<typename T2> const bool operator<(const T2& t) const { return value<t; }
		template<typename T2> const bool operator<=(const T2& t) const { return value<=t; }

		// Increment , Decrement
		const AutoValue& operator++() { ++value; return *this; }
		const TYPE& operator++(int) { return value++; }
		const AutoValue& operator--() { --value; return *this; }
		const TYPE& operator--(int) { return value--; }

		// Pointers
		TYPE* operator&() { return &value; }
		const TYPE* operator&() const { return &value; }
		typename ExtractPtr<TYPE>::Type& operator*() { return *value; }
		const typename ExtractPtr<TYPE>::Type& operator*() const { return *value; }
		typename ExtractPtr<TYPE>::Type& operator->() { return *value; }
		const typename ExtractPtr<TYPE>::Type& operator->() const { return *value; }

		// Array
		TYPE& operator[](int index) { return value[index]; }
		const TYPE& operator[](int index) const { return value[index]; }

		// Functions
		void Reset() { value= defaultValue; }
	};	

} // Fat Rat Library

#endif /* FRL_AUTO_VALUE_H_ */
