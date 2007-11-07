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
				static const int maxMsgSize = 1024+1;
				TCHAR tsMsg[maxMsgSize];
				memset(tsMsg, 0, sizeof(tsMsg));

				DWORD dwResult = ::FormatMessage(
					FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
					::GetModuleHandle(NULL), 
					error,
					langID, 
					tsMsg,
					maxMsgSize-1,
					NULL
					);

				// attempt to load string from system message table.
				if (dwResult == 0)
				{
					dwResult = ::FormatMessage(
						FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, 
						error,
						langID, 
						tsMsg,
						maxMsgSize-1,
						NULL 
						);
				}
				return (String)tsMsg;
			}
			#endif /* FRL_PLATFORM_WIN32 */
		}
	}
}
