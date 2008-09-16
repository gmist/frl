#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <math.h>
#include "os/win32/com/frl_os_win32_com_variant.h"
#include "opc/frl_opc_util.h"
#include "frl_exception.h"

namespace frl{ namespace os{ namespace win32{ namespace com{

using namespace opc;
	
Variant::Variant()
{
	init();
	value.vt = VT_EMPTY;
}

Variant::Variant( const Variant &rVal )
{
	init();
	*this = rVal;
}

Variant::Variant( bool cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( char cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( unsigned char cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( short cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( unsigned short cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( int cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( unsigned int cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( long cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( unsigned long cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( __int64 cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( unsigned __int64 cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( float cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( double cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( CY cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( const FILETIME &cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( const String& cValue )
{
	init();
	*this = cValue;
}

Variant::Variant( const VARIANT& cValue )
{
	init();
	*this = cValue;
}

Variant::~Variant()
{
	clear();
}

void Variant::init()
{
	os::win32::com::zeroMemory( &value );
}

void Variant::clear()
{
	Variant::variantClear( &value );
}

void Variant::variantClear( VARIANT *ptr )
{
	if( ptr == NULL )
		return;
	::VariantClear( ptr );
	os::win32::com::zeroMemory( ptr );
}

const VARTYPE& Variant::getType() const
{
	return value.vt;
}

VARIANT& Variant::getRef() 
{
	return value;
}

VARIANT* Variant::getPtr()
{
	return &value;
}

HRESULT Variant::variantCopy( VARIANT *dst, const VARIANT *src )
{
	if( dst == NULL || src == NULL )
		return E_FAIL;
	::VariantClear( dst );
	return ::VariantCopy( dst, (VARIANT*)src );
}

HRESULT Variant::variantCopy( VARIANT &dst, const VARIANT &src )
{
	::VariantClear( &dst );
	return ::VariantCopy( &dst, (VARIANT*)&src );
}

Variant& Variant::operator=( const Variant &rVal )
{
	if( this == &rVal )
		return *this;
	clear();
	Variant::variantCopy( &value, &rVal.value );
	return *this;
}

Variant& Variant::operator=( bool rVal )
{
	clear();
	value.vt = VT_BOOL;
	value.boolVal = ( rVal ) ? VARIANT_TRUE : VARIANT_FALSE;
	return *this;
}		

Variant& Variant::operator=( char rVal )
{
	clear();
	value.vt = VT_I1;
	value.cVal = rVal;
	return *this;
}

Variant& Variant::operator=( unsigned char rVal )
{
	clear();
	value.vt = VT_UI1;
	value.bVal = rVal;
	return *this;
}

Variant& Variant::operator=( short rVal )
{
	clear();
	value.vt = VT_I2;
	value.iVal = rVal;
	return *this;
}

Variant& Variant::operator=( unsigned short rVal )
{
	clear();
	value.vt = VT_UI2;
	value.uiVal = rVal;
	return *this;
}

Variant& Variant::operator=( int rVal )
{
	clear();
	value.vt = VT_I4;
	value.lVal = rVal;
	return *this;
}

Variant& Variant::operator=( unsigned int rVal )
{
	clear();
	value.vt = VT_UI4;
	value.ulVal = rVal;
	return *this;
}

Variant& Variant::operator=( long rVal )
{
	clear();
	value.vt = VT_I4;
	value.lVal = rVal;
	return *this;
}

Variant& Variant::operator=( unsigned long rVal )
{
	clear();
	value.vt = VT_UI4;
	value.ulVal = rVal;
	return *this;
}

Variant& Variant::operator=( __int64 rVal )
{
	clear();
	value.vt = VT_I8;
	value.llVal = rVal;
	return *this;
}

Variant& Variant::operator=( unsigned __int64 rVal )
{
	clear();
	value.vt = VT_UI8;
	value.llVal = rVal;
	return *this;
}

Variant& Variant::operator=( float rVal )
{
	clear();
	value.vt = VT_R4;
	value.fltVal = rVal;
	return *this;
}

Variant& Variant::operator=( double rVal )
{
	clear();
	value.vt = VT_R8;
	value.dblVal = rVal;
	return *this;
}

Variant& Variant::operator=( const CY &rVal )
{
	clear();
	value.vt = VT_CY;
	value.cyVal = rVal;
	return *this;
}

Variant& Variant::operator=( const FILETIME &rVal )
{
	clear();
	value.vt = VT_DATE;
	SYSTEMTIME st;
	if( ! :: FileTimeToSystemTime( &rVal, &st ) )
	{
		FRL_THROW_SYSAPI_S();
	}
	if( ! ::SystemTimeToVariantTime( &st, &value.date ) )
	{
		FRL_THROW_S();
	}
	return *this;
}

Variant& Variant::operator=( const frl::String& rVal )
{
	clear();
	value.vt = VT_BSTR;
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
	value.bstrVal = SysAllocString( rVal.c_str() );
	#else
	value.bstrVal = SysAllocString( string2wstring( rVal ).c_str() );
	#endif
	return *this;
}

Variant& Variant::operator=( const VARIANT& rVal )
{
	clear();
	variantCopy( &value, &rVal );
	return *this;
}

Variant::operator bool() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_BOOL )
		FRL_THROW_S();
	return ( value.boolVal ) ? true : false;
}

Variant::operator char() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_I1 )
		FRL_THROW_S();
	return value.cVal;
}

Variant::operator unsigned char() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_UI1 )
		FRL_THROW_S();
	return value.bVal;
}

Variant::operator short() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_I2 )
		FRL_THROW_S();
	return value.iVal;
}

Variant::operator unsigned short() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_UI2 )
		FRL_THROW_S();
	return value.uiVal;
}

Variant::operator int() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_I4 )
		FRL_THROW_S();
	return value.lVal;
}

Variant::operator unsigned int() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_UI4 )
		FRL_THROW_S();
	return ( unsigned int )value.ullVal;
}

Variant::operator long() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_I4 )
		FRL_THROW_S();
	return value.lVal;
}

Variant::operator unsigned long() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_UI4 )
		FRL_THROW_S();
	return value.ulVal;
}

Variant::operator __int64() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_I8 )
		FRL_THROW_S();
	return value.llVal;
}

Variant::operator unsigned __int64() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_UI8 )
		FRL_THROW_S();
	return value.ullVal;
}

Variant::operator float() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_R4 )
		FRL_THROW_S();
	return value.fltVal;
}

Variant::operator double() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_R8 )
		FRL_THROW_S();
	return value.dblVal;
}

Variant::operator CY() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_CY )
		FRL_THROW_S();
	return value.cyVal;
}

Variant::operator FILETIME() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_DATE )
		FRL_THROW_S();
	SYSTEMTIME st;
	if( ! ::VariantTimeToSystemTime( value.date, &st ) )
	{
		FRL_THROW_S();
	}
	FILETIME ft;
	if( ! ::SystemTimeToFileTime( &st, &ft ) )
	{
		FRL_THROW_SYSAPI_S();
	}
	return ft;
}

Variant::operator String() const
{
	FRL_EXCEPT_GUARD();
	if( value.vt != VT_BSTR )
		FRL_THROW_S();
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		return value.bstrVal;
	#else
		return wstring2string( value.bstrVal );
	#endif
}

Variant::operator const VARIANT&() const
{
	return value;
}

bool Variant::isEqual( const VARIANT &val1, const VARIANT &val2 )
{
	if ( val1.vt != val2.vt )
		return false;

	if (val1.vt & VT_ARRAY)
	{
		FRL_THROW_S(); // TODO
	}

	switch (val1.vt)
	{
	case VT_EMPTY:
		return val1.vt == val2.vt;
	case VT_I1:
		return val1.cVal == val2.cVal;
	case VT_UI1:
		return val1.bVal == val2.bVal;
	case VT_I2:
		return val1.iVal == val2.iVal;
	case VT_UI2:
		return val1.uiVal == val2.uiVal;
	case VT_I4:
		return val1.lVal == val2.lVal;
	case VT_UI4:
		return val1.ulVal == val2.ulVal;
	case VT_I8:
		return val1.llVal == val2.llVal;
	case VT_UI8:
		return val1.ullVal == val2.ullVal;
	case VT_R4:
		return val1.fltVal == val2.fltVal;
	case VT_R8:
		return val1.dblVal == val2.dblVal;
	case VT_CY:
		return (val1.cyVal.int64 == val2.cyVal.int64);
	case VT_DATE:
		return val1.date == val2.date;
	case VT_BOOL:
		return val1.boolVal == val2.boolVal;
	case VT_BSTR:
		if( val1.bstrVal != NULL && val2.bstrVal != NULL )
			return wcscmp(val1.bstrVal, val2.bstrVal) == 0;
		return ( val1.bstrVal == val2.bstrVal );
	}
	return false;
}

void Variant::setType( VARTYPE type )
{
	::VariantChangeType( &value, &value, 0, type );
}

HRESULT Variant::copyTo( VARIANT &dst ) const
{
	return Variant::variantCopy( dst, value );
}

HRESULT Variant::copyTo( VARIANT *dst ) const
{
	return Variant::variantCopy( dst, &value );
}

} // namespace com
} // namespace win32
} // namespace os
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
