#ifndef frl_lexical_cast_h_
#define frl_lexical_cast_h_

#include "stream_std/frl_sstream.h"
#include "frl_empty_type.h"
#include <algorithm>
#include <functional>
#include <locale>

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <stdio.h>
#include "frl_exception.h"
#endif // FRL_PLATFORM_WIN32

namespace frl
{
template
<	
	typename From, 
	typename To
>
To lexicalCast( const From &from )
{
	frl::stream_std::StringStream ss;
	ss << from;
	To to;
	ss.seekg( 0 );
	ss >> to;
	return to;
}

template
<	
typename From, 
typename To,
class Mutator
>
To lexicalCast( const From &from, const Mutator& mutator )
{
	frl::stream_std::StringStream ss;
	mutator( ss, from );
	To to;
	ss.seekg( 0 );
	ss >> to;
	return to;
}


namespace lexicalMutators
{

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

struct lower
{
	struct ToLowerF : public std::unary_function< Char, Char >
	{
	public:
		ToLowerF( const std::locale& Loc )
			:	m_Loc( Loc ) 
		{
		}

		#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		ToLowerF& operator = ( const ToLowerF& ){} // for make compiler happy (C4512 )
		#endif

		Char operator ()( Char Ch ) const
		{
			return std::tolower( Ch, m_Loc );
		}
	private:
		const std::locale& m_Loc;
	};
};

struct upper
{
	struct ToUpperF : public std::unary_function< Char, Char >
	{
	public:
		ToUpperF( const std::locale& Loc )
			:	m_Loc( Loc ) 
		{
		}

		#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		ToUpperF& operator = ( const ToUpperF& ){} // for make compiler happy (C4512)
		#endif

		Char operator ()( Char Ch ) const
		{
			return std::toupper( Ch, m_Loc );
		}
	private:
		const std::locale& m_Loc;
	};
};
}

template
<>
inline
String lexicalCast( const String &from, const lexicalMutators::lower& )
{
	static const std::locale &loc = std::locale();
	String result;
	result.resize( from.length() );
	std::transform( from.begin(), from.end(), result.begin(), lexicalMutators::lower::ToLowerF( loc ) ); 
	return result; 
}

template
<>
inline
String lexicalCast( const String &from, const lexicalMutators::upper& )
{
	static const std::locale &loc = std::locale();
	String result;
	result.resize( from.length() );
	std::transform( from.begin(), from.end(), result.begin(), lexicalMutators::upper::ToUpperF( loc ) ); 
	return result; 
}

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

template
<>
inline
String lexicalCast( const GUID &from )
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

template
<>
inline
GUID lexicalCast( const frl::String &from )
{

	if( from.size() != 38 || from[0] != '{' || from[9] != '-'  || from[14] != '-' || from[19] != '-' || from[24] != '-' || from[37] != '}' )
		FRL_THROW( FRL_STR("Guid string is invalid!" ) );
	
	GUID ret;
	frl::String data;

	for( int i = 1; i< 9; ++i )
	{
		data+=from[i];
	}			
	ret.Data1 = lexicalCast<frl::String, unsigned long >( data, lexicalMutators::hex() );
	data.erase();
	for( int i = 10; i< 14; ++i )
	{
		data+=from[i];
	}
	ret.Data2 = lexicalCast<frl::String, unsigned short >( data, lexicalMutators::hex() );
	data.erase();
	for( int i = 15; i< 19; ++i )
	{
		data+=from[i];
	}
	ret.Data3 = lexicalCast< frl::String, unsigned short >( data,  lexicalMutators::hex() );
	data.erase();
	for( int i = 20; i< 22; ++i )
	{
		data+=from[i];
	}
	ret.Data4[0] = (unsigned char) lexicalCast< frl::String, unsigned int >( data, lexicalMutators::hex() );
	
	data.erase();
	for( int i = 22; i< 24; ++i )
	{
		data+=from[i];
	}
	ret.Data4[1] = (unsigned char) lexicalCast<frl::String, unsigned int >( data, lexicalMutators::hex() );

	data.erase();
	for( int i = 25; i< 27; ++i )
	{
		data+=from[i];
	}
	ret.Data4[2] = (unsigned char) lexicalCast<frl::String, unsigned int >( data, lexicalMutators::hex() );

	data.erase();
	for( int i = 27; i< 29; ++i )
	{
		data+=from[i];
	}
	ret.Data4[3] = (unsigned char) lexicalCast<frl::String, unsigned int >( data, lexicalMutators::hex() );

	data.erase();
	for( int i = 29; i< 31; ++i )
	{
		data+=from[i];
	}
	ret.Data4[4] = (unsigned char) lexicalCast<frl::String, unsigned int >( data, lexicalMutators::hex() );

	data.erase();
	for( int i = 31; i< 33; ++i )
	{
		data+=from[i];
	}
	ret.Data4[5] = (unsigned char) lexicalCast<frl::String, unsigned int >( data, lexicalMutators::hex() );

	data.erase();
	for( int i = 33; i< 35; ++i )
	{
		data+=from[i];
	}
	ret.Data4[6] = (unsigned char) lexicalCast<frl::String, unsigned int >( data, lexicalMutators::hex() );

	data.erase();
	for( int i = 35; i< 37; ++i )
	{
		data+=from[i];
	}
	ret.Data4[7] = (unsigned char) lexicalCast<frl::String, unsigned int >( data, lexicalMutators::hex() );

	return ret;

}

#endif // FRL_PLATFORM_WIN32
} // FatRat Library

#endif // frl_lexical_cast_h_
