#ifndef frl_opc_com_variant_h__
#define frl_opc_com_variant_h__
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <wtypes.h>
#include "opc/frl_opc_util.h"
#include "frl_exception.h"

namespace frl
{
	namespace opc
	{
		class ComVariant
		{
		private:
			VARIANT value;
		public:
			void* operator new( size_t nSize )
			{
				FRL_EXCEPT_GUARD();
				return util::allocMemory( nSize );
			}
			void operator delete(void* p)
			{
				FRL_EXCEPT_GUARD();
				util::freeMemory( p );
			}

			// Constructors
			ComVariant();
			ComVariant( const ComVariant &rVal );

			ComVariant( bool cValue );
			ComVariant( char cValue );
			ComVariant( unsigned char cValue );
			ComVariant( short cValue);
			ComVariant( unsigned short cValue );
			ComVariant( int cValue);
			ComVariant( unsigned int cValue );
			ComVariant( long cValue );
			ComVariant( unsigned long cValue );
			ComVariant( __int64 cValue );
			ComVariant( unsigned __int64 cValue );
			ComVariant( float cValue );
			ComVariant( double cValue );
			ComVariant( CY cValue );
			ComVariant( const FILETIME &cValue );
			ComVariant( const String& cValue );
			ComVariant( const VARIANT& cValue );

			// Destructor
			~ComVariant();

			// methods
			void init();
			void clear();
			VARTYPE getType();
			void setType( VARTYPE type );
			VARIANT& getRef();
			VARIANT* getPtr();

			// assignment methods
			ComVariant& operator = ( const ComVariant &rVal );
			ComVariant& operator = ( bool rVal );
			ComVariant& operator = ( char rVal );
			ComVariant& operator = ( unsigned char rVal );
			ComVariant& operator = ( short rVal );
			ComVariant& operator = ( unsigned short rVal );
			ComVariant& operator = ( int rVal );
			ComVariant& operator = ( unsigned int rVal );
			ComVariant& operator = ( long rVal );
			ComVariant& operator = ( unsigned long rVal );
			ComVariant& operator = ( __int64 rVal );
			ComVariant& operator = ( unsigned __int64 rVal );
			ComVariant& operator = ( float rVal );
			ComVariant& operator = ( double rVal );
			ComVariant& operator = ( const CY &rVal );
			ComVariant& operator = ( const FILETIME &rVal );
			ComVariant& operator = ( const frl::String &rVal );
			ComVariant& operator = ( const VARIANT &rVal );

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
		};
	} // namespace opc
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_variant_h__
