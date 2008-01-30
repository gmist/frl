#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_group_item.h"
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/address_space/frl_opc_address_space.h"

namespace frl
{
	namespace opc
	{
		GroupItem::GroupItem()
		{
			Init();
		}

		/*GroupItem::GroupItem( const GroupItem &item_ )
		{
			serverHandle = util::getUniqueServerHandle();
			clientHandle = item_.clientHandle;
			actived = item_.actived;
			accessPath = item_.accessPath;
			itemID = item_.itemID;
			requestDataType = item_.requestDataType;
		}*/

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

		const ComVariant& GroupItem::readValue()
		{
			address_space::Tag *tag = opcAddressSpace.getLeaf( itemID );
			cachedValue = tag->read();
			lastChange = tag->getTimeStamp();
			return cachedValue;
		}

		HRESULT GroupItem::writeValue( const VARIANT &newValue )
		{
			address_space::Tag *tag;
			try
			{
				tag = opcAddressSpace.getLeaf( itemID );
			}
			catch( frl::opc::address_space::NotExistTag &ex )
			{
				ex.~NotExistTag();
				return OPC_E_INVALIDHANDLE;
			}
			VARIANT tmp;
			::VariantInit( &tmp );
			ComVariant::variantCopy( &tmp, &newValue );
			::VariantChangeType( &tmp, &tmp, 0, tag->getCanonicalDataType() );
			tag->write( tmp );
			return S_OK;
		}

		const FILETIME& GroupItem::getTimeStamp()
		{
			return lastChange;
		}

		DWORD GroupItem::getAccessRights()
		{
			address_space::Tag *tag = opcAddressSpace.getLeaf( itemID );
			return tag->getAccessRights();
		}

		DWORD GroupItem::getQuality()
		{
			address_space::Tag *tag = opcAddressSpace.getLeaf( itemID );
			return tag->getQuality();
		}

		OPCHANDLE GroupItem::getServerHandle()
		{
			return serverHandle;
		}

		frl::Bool GroupItem::isChange()
		{
			address_space::Tag *tag = opcAddressSpace.getLeaf( itemID );
			FILETIME tmp = tag->getTimeStamp();
			return ( ( lastChange.dwHighDateTime != tmp.dwHighDateTime)
						|| ( lastChange.dwLowDateTime != tmp.dwLowDateTime ) );
		}

		GroupItem* GroupItem::cloneFrom( const GroupItem &rhv )
		{
			GroupItem *grItem= new GroupItem();
			grItem->serverHandle = util::getUniqueServerHandle();
			grItem->clientHandle = rhv.clientHandle;
			grItem->actived = rhv.actived;
			grItem->accessPath = rhv.accessPath;
			grItem->itemID = rhv.itemID;
			grItem->requestDataType = rhv.requestDataType;
			return grItem;
		}

		const ComVariant& GroupItem::getCachedValue()
		{
			return cachedValue;
		}
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
