#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_group.h"
#include "opc/frl_opc_util.h"

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

			if( iid == __uuidof( IOPCAsyncIO2 ) )
			{
				*ppInterface = (dynamic_cast<IOPCAsyncIO2*>(this));
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
			if( ippUnknown == NULL) return E_POINTER;
				*ippUnknown = NULL;
				
				Group* pObject = new Group();
				pObject->clsid = pClsid;
				HRESULT hResult = pObject->QueryInterface(IID_IUnknown, (void**)ippUnknown);
				pObject->Release();
				return hResult;
		}

		REFCLSID Group::GetCLSID()
		{
			return *clsid;
		}

		Group::Group()
			: refCount( 0 )
		{
			Init();
			name = util::getUniqueName();
		}

		Group::Group( const String &groupName )
			: name( groupName ), refCount( 0 )
		{
			Init();
		}

		Group::Group( const Group &group )
			: refCount( 0 )
		{
			name = group.name;
			actived = group.actived;
			clsid = group.clsid;
			deadband = group.deadband;
			enabled = group.enabled;
			itemList = group.itemList;
			keepAlive = group.keepAlive;
			lastUpdate = group.lastUpdate;
			localeID = group.localeID;
			server = group.server;
			serverHandle = group.serverHandle;
			tickOffset = group.tickOffset;
			timeBias = group.timeBias;
			updateRate = group.updateRate;
			clientHandle = group.clientHandle;
			deleted = group.deleted;
		}

		Group::~Group()
		{
		}

		void Group::Init()
		{
			lock::Mutex::ScopeGuard scopeGuard( groupGuard );
			serverHandle = util::getUniqueServerHandle();
			clientHandle = NULL;

			actived = False;
			enabled = True;
			deleted = False;
			updateRate = 0;
			timeBias = 0;
			deadband = 0;
			localeID = LOCALE_NEUTRAL;
			keepAlive = 0;
			lastUpdate = util::getFileTime();
			tickOffset = -1;
		}

		void Group::setServerHandle( OPCHANDLE handle )
		{
			serverHandle = handle;
		}

		void Group::setServerPtr( OPCServer *serverPtr )
		{
			server = serverPtr;
		}

		OPCHANDLE Group::getServerHandle()
		{
			return serverHandle;
		}

		const String Group::getName()
		{
			return name;
		}

		LONG Group::getRefCount()
		{
			return refCount;
		}

		frl::Bool Group::isDeleted()
		{
			return deleted;
		}

		void Group::isDeleted( Bool deleteFlag )
		{
			deleted = deleteFlag;
		}

		void Group::setName( const String &newName )
		{
			name = newName;
		}
		
	} //namespace opc
} //namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
