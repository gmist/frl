#ifndef frl_opc_async_request_h_
#define frl_opc_async_request_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <list>
#include <vector>
#include "..\dependency\vendors\opc_foundation\opcda.h"
#include "frl_types.h"
#include "frl_exception.h"
#include "os/win32/com/frl_os_win32_com_variant.h"
#include "frl_smart_ptr.h"


namespace frl
{
namespace opc
{
class AsyncRequest
{
private:
	DWORD id;
	DWORD cancelID;
	Bool cancelled;
	std::list< OPCHANDLE > handles;
	std::vector< os::win32::com::Variant > values;
	DWORD source;
	OPCHANDLE groupHandle;
public:
	FRL_EXCEPTION_CLASS( InvalidParameter );
	AsyncRequest( OPCHANDLE groupHandle_ );
	AsyncRequest( OPCHANDLE groupHandle_, const std::list< OPCHANDLE > &handles_ );
	AsyncRequest( const AsyncRequest &request );
	~AsyncRequest();
	void setTransactionID( DWORD id_ );
	DWORD getTransactionID() const;
	DWORD getCancelID();
	Bool isCancelled();
	void isCancelled( Bool isCancelled_ );
	void init( const std::list< OPCHANDLE > &handles_ );
	void init( const std::list< OPCHANDLE > &handles_, const VARIANT *values_ );
	const std::list<OPCHANDLE>& getHandles() const;
	size_t getCounts() const;
	const std::vector< os::win32::com::Variant >& getValues() const;
	void removeHandle( OPCHANDLE handle );
	DWORD getSource() const;
	void setSource( DWORD source_ );
	static DWORD getUniqueCancelID();
	AsyncRequest& operator = ( const AsyncRequest &rvl );
	void swap( AsyncRequest &req );
	OPCHANDLE getGroupHandle();
}; // class AsyncRequest

typedef frl::SmartPtr< AsyncRequest, frl::smart_ptr::OwnerRefCount > AsyncRequestListElem;
typedef std::list< AsyncRequestListElem > AsyncRequestList;

} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM == FRL_PLATFORM_WIN32
#endif // frl_opc_async_request_h_
