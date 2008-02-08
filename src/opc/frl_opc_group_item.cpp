#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_group_item.h"
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/address_space/frl_opc_address_space.h"

using namespace frl::opc::address_space;

namespace frl
{
	namespace opc
	{
		GroupItem::GroupItem()
		{
			Init();
		}

		GroupItem::~GroupItem()
		{

		}

		void GroupItem::Init()
		{
			serverHandle = 0;
			clientHandle = 0;
			itemID = FRL_STR("");
			accessPath = FRL_STR("");
			actived = False;
			requestDataType = VT_EMPTY;
			tagRef = NULL;
		}

		void GroupItem::Init( OPCHANDLE serverHandle_, OPCITEMDEF &itemDef )
		{
			Init();
			serverHandle = serverHandle_;
			clientHandle = itemDef.hClient;
			itemID = itemDef.szItemID;
			accessPath = itemDef.szAccessPath;
			actived = ( itemDef.bActive == TRUE || itemDef.bActive == VARIANT_TRUE );
			requestDataType = itemDef.vtRequestedDataType;
		}

		void GroupItem::isActived( Bool activedFlag )
		{
			actived = activedFlag;
		}

		frl::Bool GroupItem::isActived()
		{
			return actived;
		}
		void GroupItem::setClientHandle( OPCHANDLE handle )
		{
			clientHandle = handle;
		}

		void GroupItem::setRequestDataType( VARTYPE type )
		{
			requestDataType = type;
		}

		OPCHANDLE GroupItem::getClientHandle()
		{
			return clientHandle;
		}

		const String& GroupItem::getItemID()
		{
			return itemID;
		}

		const String& GroupItem::getAccessPath()
		{
			return accessPath;
		}

		VARTYPE GroupItem::getReguestDataType()
		{
			return requestDataType;
		}

		const os::win32::com::Variant& GroupItem::readValue()
		{
			if( tagRef == NULL )
				tagRef = opcAddressSpace.getLeaf( itemID );

			cachedValue = tagRef->read();
			lastChange = tagRef->getTimeStamp();
			return cachedValue;
		}

		HRESULT GroupItem::writeValue( const VARIANT &newValue )
		{
			try
			{
				if( tagRef == NULL )
					tagRef = opcAddressSpace.getLeaf( itemID );
			}
			catch( Tag::NotExistTag &ex )
			{
				ex.~NotExistTag();
				return OPC_E_INVALIDHANDLE;
			}
			VARIANT tmp;
			::VariantInit( &tmp );
			os::win32::com::Variant::variantCopy( &tmp, &newValue );
			HRESULT result = ::VariantChangeType( &tmp, &tmp, 0, tagRef->getCanonicalDataType() );
			if( FAILED( result) )
				return result;
			tagRef->write( tmp );
			return S_OK;
		}

		const FILETIME& GroupItem::getTimeStamp()
		{
			return lastChange;
		}

		DWORD GroupItem::getAccessRights()
		{
			if( tagRef == NULL )
				tagRef = opcAddressSpace.getLeaf( itemID );

			return tagRef->getAccessRights();
		}

		DWORD GroupItem::getQuality()
		{
			if( tagRef == NULL )
				tagRef = opcAddressSpace.getLeaf( itemID );

			return tagRef->getQuality();
		}

		OPCHANDLE GroupItem::getServerHandle()
		{
			return serverHandle;
		}

		frl::Bool GroupItem::isChange()
		{
			if( tagRef == NULL )
				tagRef = opcAddressSpace.getLeaf( itemID );

			FILETIME tmp = tagRef->getTimeStamp();
			return ( ( lastChange.dwHighDateTime != tmp.dwHighDateTime)
						|| ( lastChange.dwLowDateTime != tmp.dwLowDateTime ) );
		}

		GroupItem* GroupItem::clone()
		{
			GroupItem *grItem= new GroupItem();
			grItem->serverHandle = util::getUniqueServerHandle();
			grItem->clientHandle = clientHandle;
			grItem->actived = actived;
			grItem->accessPath = accessPath;
			grItem->itemID = itemID;
			grItem->requestDataType = requestDataType;
			return grItem;
		}

		const os::win32::com::Variant& GroupItem::getCachedValue()
		{
			return cachedValue;
		}
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
