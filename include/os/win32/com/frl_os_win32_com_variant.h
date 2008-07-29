#ifndef frl_os_win32_com_variant_h_
#define frl_os_win32_com_variant_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <wtypes.h>
#include "opc/frl_opc_util.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "frl_exception.h"

namespace frl{ namespace os{ namespace win32{ namespace com{

class Variant : public Allocator
{
private:
	VARIANT value;
public:
	// Constructors
	Variant();
	Variant( const Variant &rVal );

	Variant( bool cValue );
	Variant( char cValue );
	Variant( unsigned char cValue );
	Variant( short cValue);
	Variant( unsigned short cValue );
	Variant( int cValue);
	Variant( unsigned int cValue );
	Variant( long cValue );
	Variant( unsigned long cValue );
	Variant( __int64 cValue );
	Variant( unsigned __int64 cValue );
	Variant( float cValue );
	Variant( double cValue );
	Variant( CY cValue );
	Variant( const FILETIME &cValue );
	Variant( const String& cValue );
	Variant( const VARIANT& cValue );

	// Destructor
	~Variant();

	// methods
	void init();
	void clear();
	const VARTYPE& getType() const;
	void setType( VARTYPE type );
	VARIANT& getRef();
	VARIANT* getPtr();

	// assignment methods
	Variant& operator = ( const Variant &rVal );
	Variant& operator = ( bool rVal );
	Variant& operator = ( char rVal );
	Variant& operator = ( unsigned char rVal );
	Variant& operator = ( short rVal );
	Variant& operator = ( unsigned short rVal );
	Variant& operator = ( int rVal );
	Variant& operator = ( unsigned int rVal );
	Variant& operator = ( long rVal );
	Variant& operator = ( unsigned long rVal );
	Variant& operator = ( __int64 rVal );
	Variant& operator = ( unsigned __int64 rVal );
	Variant& operator = ( float rVal );
	Variant& operator = ( double rVal );
	Variant& operator = ( const CY &rVal );
	Variant& operator = ( const FILETIME &rVal );
	Variant& operator = ( const frl::String &rVal );
	Variant& operator = ( const VARIANT &rVal );

	// cast methods
	operator bool() const;
	operator char() const;
	operator unsigned char() const;
	operator short() const;
	operator unsigned short() const;
	operator int() const;
	operator unsigned int() const;
	operator long() const;
	operator unsigned long() const;
	operator __int64() const;
	operator unsigned __int64() const;
	operator float() const;
	operator double() const;
	operator CY() const;
	operator FILETIME() const;
	operator String() const;
	operator const VARIANT&() const;

	// other methods
	HRESULT copyTo( VARIANT &dst ) const;
	HRESULT copyTo( VARIANT *dst ) const;

	// static methods
	static void variantClear( VARIANT *ptr );
	static HRESULT variantCopy( VARIANT *dst, const VARIANT *src );
	static HRESULT variantCopy( VARIANT &dst, const VARIANT &src );
	static bool isEqual( const VARIANT &val1, const VARIANT &val2 );
}; // class Variant

} // namespace com
} // namespace win32
} // namespace os
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_variant_h__
