#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_request_manager.h"
#include "opc/frl_opc_group_manager.h"
#include "opc/frl_opc_group.h"
#include "opc/frl_opc_async_request.h"

namespace frl
{
namespace opc
{

RequestManager::RequestManager()
	:	thr_group( new boost::thread_group() )
{
}

RequestManager::~RequestManager()
{
	boost::mutex::scoped_lock lock( scopeGuard );
	thr_group->join_all();
	delete thr_group;
}

void RequestManager::addReadRequest( AsyncRequestListElem& request )
{
	{
		boost::mutex::scoped_lock lock( scopeGuard );
		forceCleanup();
		request_map.insert( std::pair< OPCHANDLE, AsyncRequestListElem >( request->getCancelID(), request ) );
	}
	boost::function < void ( AsyncRequestListElem ) > tmpMethod = boost::bind( &RequestManager::doAsyncRead, this, _1 );
	thr_group->create_thread( boost::bind( tmpMethod, request ) );
}

void RequestManager::addWriteRequest( AsyncRequestListElem& request )
{
	{
		boost::mutex::scoped_lock lock( scopeGuard );
		forceCleanup();
		request_map.insert( std::pair< OPCHANDLE, AsyncRequestListElem >( request->getCancelID(), request ) );
	}
	boost::function < void ( AsyncRequestListElem ) > tmpMethod = boost::bind( &RequestManager::doAsyncWrite, this, _1 );
	thr_group->create_thread( boost::bind( tmpMethod, request ) );
}

bool RequestManager::cancelRequest( OPCHANDLE handle )
{
	boost::mutex::scoped_lock lock( scopeGuard );
	std::map< OPCHANDLE, AsyncRequestListElem >::iterator it = request_map.find( handle );
	if( it == request_map.end() )
		return false;
	it->second->isCancelled( True );
	return true;
}

void RequestManager::doAsyncRead( AsyncRequestListElem& request )
{
	{
		boost::mutex::scoped_lock lock( scopeGuard );
		std::map< OPCHANDLE, AsyncRequestListElem >::iterator it = request_map.find( request->getCancelID() );
		if( it == request_map.end() )
			return;
		request_map.erase( it );
	}

	GroupElem group = request->getGroup();
	IOPCDataCallback* ipCallback = NULL;
	HRESULT hResult = group->getCallback( IID_IOPCDataCallback, (IUnknown**)&ipCallback );
	if( FAILED( hResult ) )
		return;

	if( request->isCancelled() )
	{
		ipCallback->OnCancelComplete( request->getTransactionID(), group->getClientHandle() );
	}
	else
	{
		if( request->getCounts() != 0 )
			group->doAsyncRead( ipCallback, request );
	}
	ipCallback->Release();
}

void RequestManager::doAsyncWrite( AsyncRequestListElem& request )
{
	{
		boost::mutex::scoped_lock lock( scopeGuard );
		std::map< OPCHANDLE, AsyncRequestListElem >::iterator it = request_map.find( request->getCancelID() );
		if( it == request_map.end() )
			return;
		request_map.erase( it );
	}

	GroupElem group = request->getGroup();
	IOPCDataCallback* ipCallback = NULL;
	HRESULT hResult = group->getCallback( IID_IOPCDataCallback, (IUnknown**)&ipCallback );
	if( FAILED( hResult ) )
		return;

	if( request->isCancelled() )
	{
		ipCallback->OnCancelComplete( request->getTransactionID(), group->getClientHandle() );
	}
	else
	{
		if( request->getCounts() != 0 )
			group->doAsyncWrite( ipCallback, request );
	}
	ipCallback->Release();
}

void RequestManager::removeItemFromRequest( OPCHANDLE item_id )
{
	boost::mutex::scoped_lock lock( scopeGuard );
	std::map< OPCHANDLE, AsyncRequestListElem >::iterator it;
	for( it = request_map.begin(); it != request_map.end(); )
	{
		it->second->removeHandle( item_id );
		if( it->second->getCounts() == 0 )
			request_map.erase( it++ );
		else
			++it;
	}
}

void RequestManager::removeGroupFromRequest( OPCHANDLE group_id )
{
	boost::mutex::scoped_lock lock( scopeGuard );
	std::map< OPCHANDLE, AsyncRequestListElem >::iterator it;
	for( it = request_map.begin(); it != request_map.end(); )
	{
		if( it->second->getGroup()->getServerHandle() == group_id )
			request_map.erase( it++ );
		else
			++it;
	}
}

void RequestManager::forceCleanup()
{
	if( thr_group->size() >= 10000 )
	{
		thr_group->join_all();
		delete thr_group;
		thr_group = new boost::thread_group();
	}
}

} // namespace opc
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
