#include "frl_opc.h"
#include "frl_lock.h"
#include "sys/frl_sys_util.h"
#include <conio.h>
#include <atlbase.h>

class COPCServerModule : public CAtlExeModuleT< COPCServerModule >
{
public :

};

COPCServerModule _AtlModule;
CComObject<frl::opc:: OPCServer> *g_Server = NULL;

class OpcClassFactory : public IClassFactory
{
private:
	frl::lock::Mutex guard;
	int  is_out_of_proc, server_inuse;
	LONG server_count;
public:

	OpcClassFactory(): is_out_of_proc(0), server_inuse(0), server_count(0)
	{
	}

	~OpcClassFactory()
	{
	}

	void serverAdd(void)
	{
		guard.Lock();
		if (is_out_of_proc) CoAddRefServerProcess();
		++server_count;
		guard.UnLock();
	}
	void serverRemove(void)
	{
		guard.Lock();
		if (is_out_of_proc)
		{
			if (0 == CoReleaseServerProcess())
				server_inuse = 0;
		}
		if (0 == --server_count && server_inuse) server_inuse = 0;
		guard.UnLock();
	}

	HRESULT CreateServer(bool create)
	{
		static frl::lock::Mutex guard;;
		frl::lock::Mutex::ScopeGuard scopeGuard( guard );

		if( create && g_Server == NULL) 
		{
			CComObject<frl::opc::OPCServer>::CreateInstance( &g_Server );
			if( g_Server )
				g_Server->AddRef();
			else 
				return E_FAIL;
		}
		if( !create && g_Server != NULL)
		{
			g_Server->Release();
			g_Server = NULL;
		}
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE CreateInstance( 
		IUnknown *pUnkOuter,
		REFIID riid,
		void **ppvObject)
	{
		serverAdd();

		if(FAILED( CreateServer( true ) ) )
			return E_FAIL;


		return g_Server->QueryInterface(riid, ppvObject);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE LockServer( 
		BOOL fLock)
	{
		return S_OK;
	}
	
	STDMETHODIMP_(ULONG) AddRef(void) { return 1; }
	STDMETHODIMP_(ULONG) Release(void) { return 1; }

	STDMETHODIMP QueryInterface(REFIID iid, LPVOID *ppInterface)
	{
		if (ppInterface == NULL)
			return E_INVALIDARG;
		if (iid == IID_IUnknown || iid == IID_IClassFactory)
		{
			*ppInterface = this;
			AddRef();
			return S_OK;
		}
		*ppInterface = NULL;
		return E_NOINTERFACE;
	}

};

int _tmain(int , _TCHAR*)
{
	using namespace frl;
	opc::DA2Server server( opc::ServerTypes::localSever32 );
	server.setCLSID( FRL_STR("{251D3C74-535C-4ea4-A0FE-5FF3921DF3BB}") );
	server.setVendor( FRL_STR("Serg Baburin") );
	server.setDriverName( FRL_STR("SERVER_TEST") );
	server.setDescription( FRL_STR("This first testing OPC server from Serg Baburin."));
	server.setVersion( 0.1 );
	server.registrerServer();
	
	HRESULT hResult = S_OK;
	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hResult))
	{
		MessageBoxW( NULL, L"Error on CoInitializeEx",  L"Error!", MB_OK |MB_ICONSTOP );
		return 1;
	}
	
	static OpcClassFactory factory;
	DWORD dw;
	hResult = ::CoRegisterClassObject( lexicalCast<frl::String,CLSID>::get(server.getCLSID()), &factory, CLSCTX_LOCAL_SERVER |
		CLSCTX_REMOTE_SERVER |
		CLSCTX_INPROC_SERVER,
		REGCLS_MULTIPLEUSE, &dw );

	if (FAILED(hResult))
	{
		MessageBoxW( NULL, L"Error on CoRegistrerClassObject",  L"Error!", MB_OK |MB_ICONSTOP );
		return 1;
	}

	frl::opc::opcAddressSpace.finalConstruct( FRL_STR("."));
	frl::opc::opcAddressSpace.addBranch( FRL_STR( "branch1" ) );
	frl::opc::opcAddressSpace.addBranch( FRL_STR( "branch2" ) );
	frl::opc::opcAddressSpace.addBranch( FRL_STR( "branch1.branch11" ) );
	frl::opc::opcAddressSpace.addLeaf( FRL_STR("rootLeaf"));
	frl::opc::opcAddressSpace.addLeaf( FRL_STR("rootLeaf1"));
	frl::opc::opcAddressSpace.addLeaf( FRL_STR( "branch1.leaf11" ) );

	factory.serverAdd();

	if( factory.CreateServer(true) != S_OK )
	{
		MessageBoxW( NULL, L"Error on CreateServer",  L"Error!", MB_OK |MB_ICONSTOP );
		return 1;
	}
		g_Server->m_ServerStatus.dwServerState = OPC_STATUS_RUNNING;
		CoFileTimeNow(&( g_Server->m_ServerStatus.ftCurrentTime));

	while(  true )
	{
		::Sleep(100);
	}
	return 0;
}
