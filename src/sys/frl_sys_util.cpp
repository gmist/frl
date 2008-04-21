#include "sys/frl_sys_util.h"

namespace frl
{
namespace sys
{
namespace util
{
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )	
String getLastErrorDescription()
{
	return getLastErrorDescription( LANGIDFROMLCID(LOCALE_NEUTRAL) );
}

String getLastErrorDescription( WORD langID )
{
	return getCodeErrorDescription( langID, GetLastError() );
}
String getCodeErrorDescription( DWORD error )
{
	return getCodeErrorDescription( LANGIDFROMLCID(LOCALE_NEUTRAL), error );
}
String getCodeErrorDescription( WORD langID, DWORD error )
{
	void *tsMsg = NULL;
	DWORD dwResult = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
		::GetModuleHandle(NULL),
		error,
		langID,
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		( LPWSTR )&tsMsg,
		#else
		( LPSTR )&tsMsg,
		#endif
		0,
		NULL
		);

	if (dwResult == 0)
	{
		dwResult = ::FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			langID, 
			#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			( LPWSTR )&tsMsg,
			#else
			( LPSTR )&tsMsg,
			#endif
			0,
			NULL
			);
	}
	String out(FRL_STR(""));
	if( tsMsg != NULL )
	{
		out = (Char*)(tsMsg);
		LocalFree(tsMsg);
	}
	return out;
}
#endif // FRL_PLATFORM_WIN32
} // namespace util
} // namespace sys
} // FatRat Library
