#ifndef FRL_LEXICAL_CAST_H_
#define FRL_LEXICAL_CAST_H_

#include "stream_std/frl_sstream.h"
#include "stream_std/frl_ostream.h"
#include "frl_empty_type.h"

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include "frl_string.h"
#include <stdio.h>
#include "frl_exception.h"
#endif

namespace frl
{

	template <		class From,
						class To = frl::String,
						class Mutator = frl::EmptyType >
	struct lexicalCast
	{
		static To get( const From &from )
		{
			frl::stream_std::StringStream ss;
			Mutator mutator;
			mutator( ss, from );
			To to;
			ss.seekg( 0 );
			ss >> to;
			return to;
		}
	};

	template < class From >
	struct lexicalCast < From, frl::String, frl::EmptyType >
	{
		static frl::String get( const From &from )
		{
			frl::stream_std::StringStream ss;
			ss << from;
			return ss.str();
		}
	};

	template < class From,
					class To >
	struct lexicalCast < From, To, frl::EmptyType >
	{
		static To get( const From &from )
		{
			frl::stream_std::StringStream ss;
			ss << from;
			To to;
			ss.seekg( 0 );
			ss >> to;
			return to;
		}
	};	

	namespace lexicalCastMutators
	{
		struct dummy
		{
			template< class T >
			void operator()( frl::stream_std::StringStream &to, const T &v ) const
			{
				to << v;
			}
		};

		struct hex
		{
			template< class T >
			void operator()( frl::stream_std::StringStream &to, const T &v ) const
			{
				to << std::hex << v;
			}
		};

		struct dec
		{
			template< class T >
			void operator()( frl::stream_std::StringStream &to, const T &v ) const
			{
				to << std::dec << v;
			}
		};

		struct oct 
		{
			template< class T >
			void operator()( frl::stream_std::StringStream &to, const T &v ) const
			{
				to << std::oct << v;
			}
		};
	}

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

	template<>
	struct lexicalCast< GUID, frl::String, frl::EmptyType >
	{
		static frl::String get( const GUID &from )
		{
			char str[40];
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			#pragma warning ( push )
			#pragma warning( disable: 4996 )
			#endif

			sprintf (str, "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
				from.Data1, from.Data2, from.Data3, from.Data4[0], from.Data4[1],
				from.Data4[2], from.Data4[3], from.Data4[4],
				from.Data4[5], from.Data4[6], from.Data4[7]);

			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			#pragma warning ( pop )
			#endif
			return unicodeIndependString( multiByteCompatibility(str) );			
		}
	};

	template<>
	struct lexicalCast< frl::String, GUID, frl::EmptyType >
	{
		static GUID get( const frl::String &from )
		{
			if( from.size() != 38 || from[0] != '{' || from[9] != '-'  || from[14] != '-' || from[19] != '-' || from[24] != '-' || from[37] != '}' )
				FRL_THROW( FRL_STR("Guid string is invalid!" ) );
			
			GUID ret;
			frl::String data;

			for ( int i = 1; i< 9; i++ )
			{
				data+=from[i];
			}			
			ret.Data1 = lexicalCast<frl::String, unsigned long, lexicalCastMutators::hex >::get( data );
			data.erase();
			for ( int i = 10; i< 14; i++ )
			{
				data+=from[i];
			}
			ret.Data2 = lexicalCast<frl::String, unsigned short, lexicalCastMutators::hex >::get( data );
			data.erase();
			for ( int i = 15; i< 19; i++ )
			{
				data+=from[i];
			}
			ret.Data3 = lexicalCast< frl::String, unsigned short, lexicalCastMutators::hex >::get( data );
			data.erase();
			for ( int i = 20; i< 22; i++ )
			{
				data+=from[i];
			}
			ret.Data4[0] = lexicalCast<frl::String, int, lexicalCastMutators::hex >::get( data );
			
			data.erase();
			for ( int i = 22; i< 24; i++ )
			{
				data+=from[i];
			}
			ret.Data4[1] = lexicalCast<frl::String, int, lexicalCastMutators::hex >::get( data );

			data.erase();
			for ( int i = 25; i< 27; i++ )
			{
				data+=from[i];
			}
			ret.Data4[2] = lexicalCast<frl::String, int, lexicalCastMutators::hex >::get( data );

			data.erase();
			for ( int i = 27; i< 29; i++ )
			{
				data+=from[i];
			}
			ret.Data4[3] = lexicalCast<frl::String, int, lexicalCastMutators::hex >::get( data );

			data.erase();
			for ( int i = 29; i< 31; i++ )
			{
				data+=from[i];
			}
			ret.Data4[4] = lexicalCast<frl::String, int, lexicalCastMutators::hex >::get( data );

			data.erase();
			for ( int i = 31; i< 33; i++ )
			{
				data+=from[i];
			}
			ret.Data4[5] = lexicalCast<frl::String, int, lexicalCastMutators::hex >::get( data );

			data.erase();
			for ( int i = 33; i< 35; i++ )
			{
				data+=from[i];
			}
			ret.Data4[6] = lexicalCast<frl::String, int, lexicalCastMutators::hex >::get( data );

			data.erase();
			for ( int i = 35; i< 37; i++ )
			{
				data+=from[i];
			}
			ret.Data4[7] = lexicalCast<frl::String, int, lexicalCastMutators::hex >::get( data );

			return ret;
		}
	};

	#endif /* FRL_PLATFORM_WIN32 */
}

#endif /* FRL_LEXICAL_CAST_H_ */
