#ifndef frl_opc_cache_item_h_
#define frl_opc_cache_item_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"

namespace frl
{
	namespace opc
	{
		class CacheItem
		{
		private:
			String id;
			String accessPath;
			VARTYPE requestedDataType, canonicalDataType;
			OPCHANDLE clientHandle, serverHandle;
			DWORD accessRights;
			Bool active;
		public:
			void setID( const String &newID );
			const String& getID();
			void setAccessPath( const String& newAccessPath );
			const String& getAccessPath();
			void setRequestedDataType( VARTYPE newType );
			VARTYPE getReguestedDataType();
			void setCanonicalDataType( VARTYPE newType );
			VARTYPE getCanonicalDataType();
			void setClientHandle( OPCHANDLE newHandle );
			OPCHANDLE getClientHandle();
			void setServerHandle( OPCHANDLE newHandle );
			OPCHANDLE getServerHandle();
			void setAccessRights( DWORD newAccessRights );
			DWORD getAccessRights();
			void isActive( Bool actived );
			Bool isActive();
			void isWriteable( Bool writeable );
			Bool isWriteable();
		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_cache_item_h_ */
