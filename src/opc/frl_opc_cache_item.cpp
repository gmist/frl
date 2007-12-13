#include "opc/frl_opc_cache_item.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

namespace frl
{
	namespace opc
	{
		CacheItem::CacheItem()
		{
			requestedDataType = canonicalDataType = VT_EMPTY;
			clientHandle = serverHandle = 0;
			accessRights = OPC_READABLE;
			active = 0;
		}

		void CacheItem::setID( const String &newID )
		{
			id = newID;
		}

		const String& CacheItem::getID()
		{
			return id;
		}

		void CacheItem::setAccessPath( const String& newAccessPath )
		{
			accessPath = newAccessPath;
		}

		const String& CacheItem::getAccessPath()
		{
			return accessPath;
		}

		void CacheItem::setRequestedDataType( VARTYPE newType )
		{
			requestedDataType = newType;
		}

		VARTYPE CacheItem::getReguestedDataType()
		{
			return requestedDataType;
		}

		void CacheItem::setCanonicalDataType( VARTYPE newType )
		{
			canonicalDataType = newType;
		}

		VARTYPE CacheItem::getCanonicalDataType()
		{
			return canonicalDataType;
		}

		void CacheItem::setClientHandle( OPCHANDLE newHandle )
		{
			clientHandle = newHandle;
		}

		OPCHANDLE CacheItem::getClientHandle()
		{
			return clientHandle;
		}

		void CacheItem::setServerHandle( OPCHANDLE newHandle )
		{
			serverHandle = newHandle;
		}

		OPCHANDLE CacheItem::getServerHandle()
		{
			return serverHandle;
		}

		void CacheItem::setAccessRights( DWORD newAccessRights )
		{
			accessRights = newAccessRights;
		}

		DWORD CacheItem::getAccessRights()
		{
			return accessRights;
		}

		void CacheItem::isActive( Bool actived )
		{
			active = actived;
		}

		frl::Bool CacheItem::isActive()
		{
			return active;
		}

		void CacheItem::isWriteable( Bool writeable )
		{
			if( writeable )
				accessRights |= OPC_WRITEABLE;
			else
				accessRights = OPC_READABLE;
		}

		frl::Bool CacheItem::isWriteable()
		{
			return ( accessRights & OPC_WRITEABLE ) == 1;
		}

		HRESULT CacheItem::read( VARIANT &retValue )
		{
			VariantInit( &retValue );
			VariantCopy( &retValue, &value );
			return S_OK;
		}
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */