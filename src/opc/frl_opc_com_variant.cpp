#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <math.h>
#include "opc/frl_opc_com_variant.h"
#include "opc/frl_opc_util.h"
#include "frl_exception.h"

namespace frl
{
	namespace opc
	{
		ComVariant::ComVariant()
		{
			init();
			value.vt = VT_EMPTY;
		}

		ComVariant::ComVariant( const ComVariant &rVal )
		{
			init();
			*this = rVal;
		}

		ComVariant::ComVariant( bool cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( char cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( unsigned char cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( short cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( unsigned short cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( int cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( unsigned int cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( long cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( unsigned long cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( __int64 cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( unsigned __int64 cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( float cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( double cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( CY cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( const FILETIME &cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( const String& cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::ComVariant( const VARIANT& cValue )
		{
			init();
			*this = cValue;
		}

		ComVariant::~ComVariant()
		{
			clear();
		}

		void ComVariant::init()
		{
			util::zeroMemory( &value );
		}

		void ComVariant::clear()
		{
			ComVariant::variantClear( &value );
		}

		void ComVariant::variantClear( VARIANT *ptr )
		{
			if( ptr == NULL )
				return;
			::VariantClear( ptr );
			util::zeroMemory( ptr );
		}

		VARTYPE ComVariant::getType()
		{
			return value.vt;
		}

		VARIANT& ComVariant::getRef() 
		{
			return value;
		}

		VARIANT* ComVariant::getPtr()
		{
			return &value;
		}

		HRESULT ComVariant::variantCopy( VARIANT *dst, const VARIANT *src )
		{
			if( dst == NULL )
				return E_FAIL;
			ComVariant::variantClear( dst );
			if( src == NULL )
				return E_FAIL;
			return ::VariantCopy( dst, (VARIANT*)src );
		}

		HRESULT ComVariant::variantCopy( VARIANT &dst, const VARIANT &src )
		{
			::VariantInit( &dst );
			return ::VariantCopy( &dst, (VARIANT*)&src );
		}

		ComVariant& ComVariant::operator=( const ComVariant &rVal )
		{
			if( this == &rVal )
				return *this;
			clear();
			ComVariant::variantCopy( &value, &rVal.value );
			return *this;
		}

		ComVariant& ComVariant::operator=( bool rVal )
		{
			clear();
			value.vt = VT_BOOL;
			value.boolVal = ( rVal ) ? VARIANT_TRUE : VARIANT_FALSE;
			return *this;
		}		

		ComVariant& ComVariant::operator=( char rVal )
		{
			clear();
			value.vt = VT_I1;
			value.cVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( unsigned char rVal )
		{
			clear();
			value.vt = VT_UI1;
			value.bVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( short rVal )
		{
			clear();
			value.vt = VT_I2;
			value.iVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( unsigned short rVal )
		{
			clear();
			value.vt = VT_UI2;
			value.uiVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( int rVal )
		{
			clear();
			value.vt = VT_I4;
			value.lVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( unsigned int rVal )
		{
			clear();
			value.vt = VT_UI4;
			value.ulVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( long rVal )
		{
			clear();
			value.vt = VT_I4;
			value.lVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( unsigned long rVal )
		{
			clear();
			value.vt = VT_UI4;
			value.ulVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( __int64 rVal )
		{
			clear();
			value.vt = VT_I8;
			value.llVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( unsigned __int64 rVal )
		{
			clear();
			value.vt = VT_UI8;
			value.llVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( float rVal )
		{
			clear();
			value.vt = VT_R4;
			value.fltVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( double rVal )
		{
			clear();
			value.vt = VT_R8;
			value.dblVal = rVal;
			return *this;
		}
			
		ComVariant& ComVariant::operator=( const CY &rVal )
		{
			clear();
			value.vt = VT_CY;
			value.cyVal = rVal;
			return *this;
		}

		ComVariant& ComVariant::operator=( const FILETIME &rVal )
		{
			clear();
			value.vt = VT_DATE;
			SYSTEMTIME st;
			FileTimeToSystemTime( &rVal, &st );
			SystemTimeToVariantTime( &st, &value.date );
			::ZeroMemory( &st, sizeof(SYSTEMTIME) );
			return *this;
		}
		
		ComVariant& ComVariant::operator=( const frl::String& rVal )
		{
			clear();
			value.vt = VT_BSTR;
			value.bstrVal = SysAllocString( rVal.c_str() );
			return *this;
		}

		ComVariant& ComVariant::operator=( const VARIANT& rVal )
		{
			clear();
			variantCopy( &value, &rVal );
			return *this;
		}

		ComVariant::operator bool() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_BOOL )
				FRL_THROW_S();
			return ( value.boolVal ) ? true : false;
		}

		ComVariant::operator char() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_I1 )
				FRL_THROW_S();
			return value.cVal;
		}

		ComVariant::operator unsigned char() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_UI1 )
				FRL_THROW_S();
			return value.bVal;
		}

		ComVariant::operator short() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_I2 )
				FRL_THROW_S();
			return value.iVal;
		}

		ComVariant::operator unsigned short() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_UI2 )
				FRL_THROW_S();
			return value.uiVal;
		}

		ComVariant::operator int() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_I4 )
				FRL_THROW_S();
			return value.lVal;
		}

		ComVariant::operator unsigned int() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_UI4 )
				FRL_THROW_S();
			return ( unsigned int )value.ullVal;
		}

		ComVariant::operator long() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_I4 )
				FRL_THROW_S();
			return value.lVal;
		}

		ComVariant::operator unsigned long() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_UI4 )
				FRL_THROW_S();
			return value.ulVal;
		}

		ComVariant::operator __int64() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_I8 )
				FRL_THROW_S();
			return value.llVal;
		}
		
		ComVariant::operator unsigned __int64() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_UI8 )
				FRL_THROW_S();
			return value.ullVal;
		}

		ComVariant::operator float() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_R4 )
				FRL_THROW_S();
			return value.fltVal;
		}
		
		ComVariant::operator double() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_R8 )
				FRL_THROW_S();
			return value.dblVal;
		}

		ComVariant::operator CY() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_CY )
				FRL_THROW_S();
			return value.cyVal;
		}

		ComVariant::operator FILETIME() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_DATE )
				FRL_THROW_S();
			SYSTEMTIME st;
			if( ! ::VariantTimeToSystemTime( value.date, &st ) )
				FRL_THROW_S();
			FILETIME ft;
			if( ! ::SystemTimeToFileTime( &st, &ft ) )
			{
				::ZeroMemory( &st, sizeof(SYSTEMTIME) );
				FRL_THROW_S();
			}
			::ZeroMemory( &st, sizeof(SYSTEMTIME) );
			return ft;
		}

		ComVariant::operator String() const
		{
			FRL_EXCEPT_GUARD();
			if( value.vt != VT_BSTR )
				FRL_THROW_S();
			return value.bstrVal;
		}

		ComVariant::operator const VARIANT&() const
		 {
			 return value;
		}

		bool ComVariant::isEqual( const VARIANT &val1, const VARIANT &val2 )
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

		void ComVariant::setType( VARTYPE type )
		{
			::VariantChangeType( &value, &value, 0, type );
		}

		HRESULT ComVariant::copyTo( VARIANT &dst ) const
		{
			return ComVariant::variantCopy( dst, value );
		}

		HRESULT ComVariant::copyTo( VARIANT *dst ) const
		{
			return ComVariant::variantCopy( dst, &value );
		}
	} // namespace opc
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
