#include "opc/frl_opc_da2_server.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server_factory.h"

namespace frl
{
	namespace opc
	{
		DA2Server::DA2Server( const ServerType& serverType )
			:	ServerKind( serverType )
		{
			isInit = False;
		}

		DA2Server::~DA2Server()
		{
			if ( isInit )
			{
				CoRevokeClassObject( objectId );
				CoUninitialize();
			}
		}

		void DA2Server::registrerServer()
		{
			ServerKind::registrerServer( ::CATID_OPCDAServer20 );
		}

		frl::Bool DA2Server::init()
		{
			HRESULT hResult = S_OK;
			hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
			if (FAILED(hResult))
			{
				MessageBoxW( NULL, L"Error on CoInitializeEx",  L"Error!", MB_OK |MB_ICONSTOP );
				isInit = False;
				return frl::False;
			}

			factory.isOutProc( True );

			hResult = ::CoRegisterClassObject( lexicalCast<frl::String,CLSID>::get( getCLSID() ), &factory, CLSCTX_LOCAL_SERVER |
				CLSCTX_REMOTE_SERVER |
				CLSCTX_INPROC_SERVER,
				REGCLS_MULTIPLEUSE, &objectId );

			if( FAILED(hResult) )
			{
				MessageBoxW( NULL, L"Error on CoRegistrerClassObject",  L"Error!", MB_OK |MB_ICONSTOP );
				CoUninitialize();
				isInit = False;
				return False;
			}			
			isInit = True;
			return True;
		}
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
