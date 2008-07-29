#include "opc/frl_opc_item_attributes.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_util.h"
#include "opc/address_space/frl_opc_address_space.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"

namespace frl
{
namespace opc
{

void ItemAttributes::freeStrings()
{
	if( attributes->szAccessPath )
		os::win32::com::freeMemory( attributes->szAccessPath );
	if( attributes->szItemID )
		os::win32::com::freeMemory( attributes->szItemID );
}

ItemAttributes::ItemAttributes()
{
	attributes = os::win32::com::allocMemory< OPCITEMATTRIBUTES >();
	os::win32::com::zeroMemory( attributes );
}

ItemAttributes::ItemAttributes( const ItemAttributes& other )
{
	attributes = os::win32::com::allocMemory< OPCITEMATTRIBUTES >();
	attributes->bActive = other.attributes->bActive;
	attributes->dwAccessRights = other.attributes->dwAccessRights;
	attributes->dwBlobSize = other.attributes->dwBlobSize;
	attributes->dwEUType = other.attributes->dwEUType;
	attributes->hClient = other.attributes->hClient;
	attributes->hServer = other.attributes->hServer;
	attributes->pBlob = other.attributes->pBlob;
	attributes->szAccessPath = util::duplicateString( other.attributes->szAccessPath );
	attributes->szItemID = util::duplicateString( other.attributes->szItemID );
	VariantCopy( &attributes->vEUInfo , &other.attributes->vEUInfo );
	attributes->vtCanonicalDataType = other.attributes->vtCanonicalDataType;
	attributes->vtRequestedDataType = other.attributes->vtRequestedDataType;
}

ItemAttributes::~ItemAttributes()
{
	freeStrings();
	os::win32::com::freeMemory( attributes );
}

ItemAttributes& ItemAttributes::operator=( const ItemAttributes& rhv )
{
	if( this == &rhv )
		return *this;

	freeStrings();
	os::win32::com::zeroMemory( attributes );

	attributes->bActive = rhv.attributes->bActive;
	attributes->dwAccessRights = rhv.attributes->dwAccessRights;
	attributes->dwBlobSize = rhv.attributes->dwBlobSize;
	attributes->dwEUType = rhv.attributes->dwEUType;
	attributes->hClient = rhv.attributes->hClient;
	attributes->hServer = rhv.attributes->hServer;
	attributes->pBlob = rhv.attributes->pBlob;
	attributes->szAccessPath = util::duplicateString( rhv.attributes->szAccessPath );
	attributes->szItemID = util::duplicateString( rhv.attributes->szItemID );
	VariantCopy( &attributes->vEUInfo , &rhv.attributes->vEUInfo );
	attributes->vtCanonicalDataType = rhv.attributes->vtCanonicalDataType;
	attributes->vtRequestedDataType = rhv.attributes->vtRequestedDataType;

	return *this;
}

ItemAttributes& ItemAttributes::operator=( const std::pair< OPCHANDLE, GroupItemElem >& newItem )
{
	freeStrings();
	os::win32::com::zeroMemory( attributes );

	attributes->hServer = newItem.first;

	attributes->bActive = newItem.second->isActived();
	attributes->hClient = newItem.second->getClientHandle();

#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
	attributes->szItemID = util::duplicateString( newItem.second->getItemID() );
#else
	attributes->szItemID = util::duplicateString( string2wstring( newItem.second->getItemID() ) );
#endif

#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
	attributes->szAccessPath = util::duplicateString( newItem.second->getAccessPath() );
#else
	attributes->szAccessPath = util::duplicateString( string2wstring( newItem.second->getAccessPath() ) );
#endif

	address_space::Tag *item = opcAddressSpace::getInstance().getTag( newItem.second->getItemID() );
	attributes->dwAccessRights = item->getAccessRights();
	attributes->dwBlobSize = 0;
	attributes->pBlob = NULL;
	attributes->vtCanonicalDataType = item->getCanonicalDataType();
	attributes->vtRequestedDataType = newItem.second->getReguestDataType();
	return *this;
}

void ItemAttributes::copyTo( OPCITEMATTRIBUTES& dst )
{
	os::win32::com::zeroMemory( &dst );
	dst.bActive = attributes->bActive;
	dst.dwAccessRights = attributes->dwAccessRights;
	dst.dwBlobSize = attributes->dwBlobSize;
	dst.dwEUType = attributes->dwEUType;
	dst.hClient = attributes->hClient;
	dst.hServer = attributes->hServer;
	dst.pBlob = attributes->pBlob;

	if( attributes->szAccessPath )
		dst.szAccessPath = util::duplicateString( attributes->szAccessPath );

	if( attributes->szItemID )
		dst.szItemID = util::duplicateString( attributes->szItemID );

	VariantCopy(&dst.vEUInfo , &attributes->vEUInfo );
	dst.vtCanonicalDataType = attributes->vtCanonicalDataType;
	dst.vtRequestedDataType = attributes->vtRequestedDataType;
}

} // namespace opc
} // FatRat Libray

#endif // FRL_PLATFORM_WIN32
