#ifndef frl_lexical_cast_h_
#define frl_lexical_cast_h_

#include "stream_std/frl_sstream.h"
#include "frl_empty_type.h"
#include <algorithm>
#include <functional>
#include <locale>

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include "frl_exception.h"
#endif // FRL_PLATFORM_WIN32

namespace frl{

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


namespace lexicalMutators{

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

} // namespace lexicalMutators

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
	WCHAR* wszCLSID = NULL ;
	HRESULT result = StringFromCLSID( from, &wszCLSID );
	if( FAILED(result) )
		FRL_THROW( FRL_STR("Can`t convert GUID to string") );
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		String toStr = wszCLSID;
	#else
		String toStr = multiByteCompatibility( wszCLSID );
	#endif
	CoTaskMemFree(wszCLSID);
	return toStr;
}

template
<>
inline
GUID lexicalCast( const frl::String &from )
{
	CLSID clsid;
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		HRESULT result = CLSIDFromString( (wchar_t*)from.c_str(), &clsid );
	#else
		std::wstring tmp = unicodeCompatibility( from );
		HRESULT result = CLSIDFromString( (wchar_t*)tmp.c_str(), &clsid );
	#endif
	if( FAILED(result) )
		FRL_THROW( FRL_STR("Can`t convert string to GUID") );
	return clsid;
}

#endif // FRL_PLATFORM_WIN32

} // FatRat Library

#endif // frl_lexical_cast_h_
