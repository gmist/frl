#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <algorithm>
#include "opc/frl_opc_group.h"
#include "opc/frl_opc_util.h"
#include "frl_util_functors.h"
#include "os/win32/com/frl_os_win32_com_uuidof.h"

namespace frl
{
namespace opc
{
HRESULT STDMETHODCALLTYPE Group::QueryInterface( /* [in] */ REFIID iid, /* [iid_is][out] */ void** ppInterface )
{
	if( ppInterface == NULL )
		return E_POINTER;

	*ppInterface = NULL;

	if( iid == __uuidof( IOPCGroupStateMgt ) || iid == IID_IUnknown )
	{
		*ppInterface = (dynamic_cast<IOPCGroupStateMgt*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCItemMgt ) )
	{
		*ppInterface = (dynamic_cast<IOPCItemMgt*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCSyncIO ) )
	{
		*ppInterface = (dynamic_cast<IOPCSyncIO*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCSyncIO2 ) )
	{
		*ppInterface = (dynamic_cast<IOPCSyncIO2*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCAsyncIO2 ) )
	{
		*ppInterface = (dynamic_cast<IOPCAsyncIO2*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCAsyncIO3 ) )
	{
		*ppInterface = (dynamic_cast<IOPCAsyncIO3*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IConnectionPointContainer ) )
	{
		*ppInterface = (dynamic_cast< IConnectionPointContainer* >(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCGroupStateMgt2 ) )
	{
		*ppInterface = (dynamic_cast< IOPCGroupStateMgt2* >(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCItemDeadbandMgt ) )
	{
		*ppInterface = (dynamic_cast< IOPCItemDeadbandMgt* >(this));
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE Group::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

ULONG STDMETHODCALLTYPE Group::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

HRESULT STDMETHODCALLTYPE Group::CreateInstance( IUnknown** ippUnknown, const CLSID* pClsid )
{
	if( ippUnknown == NULL)
		return E_POINTER;
	
	*ippUnknown = NULL;
	Group* pObject = new Group();
	pObject->clsid = pClsid;
	HRESULT hResult = pObject->QueryInterface(IID_IUnknown, (void**)ippUnknown);
	pObject->Release();
	return hResult;
}

Group::Group()
	:	refCount( 0 )
{
}

Group::Group( const String &groupName )
	:	GroupBase( groupName ),
		refCount( 0 )
{
}

Group::~Group()
{
	groupGuard.lock();
	unregisterInterface(IID_IOPCDataCallback);
	groupGuard.unlock();
}

GroupElem Group::clone()
{
	GroupElem newGroup( new Group() );
	newGroup->registerInterface(IID_IOPCDataCallback);

	newGroup->name = name;
	newGroup->actived = False;
	newGroup->clsid = clsid;
	newGroup->deadband = deadband;
	newGroup->enabled = enabled;
	newGroup->keepAlive = keepAlive;
	newGroup->lastUpdate = lastUpdate;
	newGroup->localeID = localeID;
	newGroup->server = server;
	newGroup->timeBias = timeBias;
	newGroup->updateRate = updateRate;
	newGroup->clientHandle = clientHandle;
	newGroup->deleted = deleted;

	GroupItemElemList::const_iterator end = itemList.end();
	for( GroupItemElemList::const_iterator it = itemList.begin(); it != end; ++it )
	{
		GroupItemElem item( (*it).second->clone() );
		newGroup->itemList.insert( std::pair< OPCHANDLE, GroupItemElem > ( item->getServerHandle(), item ) );
	}
	return newGroup;
}

} //namespace opc
} //namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
