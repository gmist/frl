#ifndef frl_auto_value_h_
#define frl_auto_value_h_

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

	//see http://rsdn.ru/Forum/Message.aspx?mid=174720&only=1
	// and http://rsdn.ru/Forum/Message.aspx?mid=612566&only=1
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

		// data
		const TYPE& data() const { return value; }
		TYPE& data() { return value; }

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
		void reset() { value= defaultValue; }
	};	

} // FatRat Library

#endif // frl_auto_value_h_
