#include "frl_opc.h"
#include "frl_lock.h"
#include <conio.h>
#include <iostream>
#include <atlbase.h>

class COPCServerModule : 
	public CAtlExeModuleT< COPCServerModule >
{
public :
	//DECLARE_LIBID(LIBID_XmlUtilsLib)
	//DECLARE_REGISTRY_APPID_RESOURCEID(IDR_XMLUTILS, "{0E3C0CC6-5397-49E4-8273-7DFB51B8787C}")
};

COPCServerModule _AtlModule;


CComObject<frl::opc:: OPCServer> *g_Server = NULL;

class OpcClassFactory : public IClassFactory
{
private:
	frl::lock::Mutex guard;

	int  is_out_of_proc, 
		server_inuse; /* go 0 when unloading initiated */
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

		if( create && g_Server == NULL) {
			CComObject<frl::opc::OPCServer>::CreateInstance( &g_Server );
			if( g_Server )
				g_Server->AddRef();
			else return E_FAIL;
		}
		if( !create && g_Server != NULL) {
			//g_Server->Fire_ShutdownRequest( L"Close OPC Server Application" );
			g_Server->Release();
			g_Server = NULL;
		}
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE CreateInstance( 
		/* [unique][in] */ IUnknown *pUnkOuter,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppvObject)
	{		
		serverAdd();  /* the lock for a_server_finished() */

		if(FAILED( CreateServer( true ) ) )
			return E_FAIL;


		return g_Server->QueryInterface(riid, ppvObject);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE LockServer( 
		/* [in] */ BOOL fLock)
	{
		return S_OK;
	}

	/* Do nothing: we're static, he-he */  
	STDMETHODIMP_(ULONG) AddRef(void) { return 1; }
	STDMETHODIMP_(ULONG) Release(void) { return 1; }

	STDMETHODIMP QueryInterface(REFIID iid, LPVOID *ppInterface)
	{
		if (ppInterface == NULL)
			return E_INVALIDARG;
		if (iid == IID_IUnknown || iid == IID_IClassFactory)
		{
			//          UL_DEBUG((LOGID, "myClassFactory::QueryInterface() Ok"));
			*ppInterface = this;
			AddRef();
			return S_OK;
		}
		//    UL_DEBUG((LOGID, "myClassFactory::QueryInterface() Failed"));
		*ppInterface = NULL;
		return E_NOINTERFACE;
	}

};

int _tmain(int /*argc*/, _TCHAR* /*argv[]*/)
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

	// record service control thread.
	//m_dwThread = GetCurrentThreadId();

	// intialize thread as free threaded service.
	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(hResult))
	{
		std::cout << "1" << std::endl;
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
		std::cout << "2" << std::endl;
		return 1;
	}

	
	factory.serverAdd();

	if( factory.CreateServer(true) != S_OK )
		MessageBoxW( NULL, L"Errr",  L"ERR", MB_OK );
	//SetServerState( state );

	frl::opc::flatDataCache.AddTag( FRL_STR("wee") );
	frl::opc::flatDataCache.AddTag( FRL_STR("wee1") );

	//DWORD pdwRegister[256];

	// register class objects.
/*	hResult = OpcRegisterClassObjects(
		m_pClasses, 
		CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, 
		REGCLS_MULTIPLEUSE,
		pdwRegister);*/
	//COpcClassFactory* pFactory = new COpcClassFactory((frl::opc::ServerKind*)&server);
	
	/*CComObject<frl::opc::OPCServer>::CreateInstance( &g_Server );
	if( g_Server )
		g_Server->AddRef();


	getch();*/
	while(  true )
	{
		::Sleep(100);
	}


	/*opc::ServerDA2 server( frl::opc::ServerTypes::localSever32 );
	server.setCLSID( FRL_STR("{251D3C74-535C-4ea4-A0FE-5FF3921DF3BB}") );
	server.setVendor( FRL_STR("FRT") );
	server.setDriverName( FRL_STR("SERVER_TEST") );
	server.setVersion( 0.1 );
	server.registerServer();
	server.unregisterServer();*/	
	return 0;
}