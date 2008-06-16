#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_async_request.h"
#include "os/win32/com/frl_os_win32_com_variant.h"

namespace frl
{
namespace opc
{
using namespace os::win32::com;

AsyncRequest::AsyncRequest( OPCHANDLE groupHandle_ )
	:	id( 0 ),
		cancelID( getUniqueCancelID() ),
		cancelled( False ),
		source( OPC_DS_DEVICE ),
		groupHandle( groupHandle_ )
{
}

AsyncRequest::AsyncRequest( OPCHANDLE groupHandle_, const std::list< OPCHANDLE > &handles_ )
	:	id( 0 ),
		cancelID( getUniqueCancelID() ),
		cancelled( False ),
		source( 0 ),
		groupHandle( groupHandle_ )
{
	std::list< OPCHANDLE >::const_iterator end = handles_.end();
	for( std::list< OPCHANDLE >::const_iterator it = handles_.begin(); it != end; ++it )
	{
		ItemHVQT tmp;
		tmp.setHandle( (*it) );
		itemHVQTList.push_back( tmp );
	}
}

AsyncRequest::AsyncRequest( const AsyncRequest &request )
	:	id( request.id ),
		cancelID( request.cancelID ),
		cancelled( request.cancelled ),
		itemHVQTList( request.itemHVQTList ),
		source( request.source ),
		groupHandle( request.groupHandle )
{

}

AsyncRequest::AsyncRequest( OPCHANDLE groupHandle_, const std::list< ItemHVQT >& itemsList )
	:	id( 0 ),
		cancelID( getUniqueCancelID() ),
		cancelled( False ),
		itemHVQTList( itemsList ),
		source( 0 ),
		groupHandle( groupHandle_ )
{

}

AsyncRequest::~AsyncRequest()
{
}

void AsyncRequest::setTransactionID( DWORD id_ )
{
	id = id_;
}

DWORD AsyncRequest::getTransactionID() const
{
	return id;
}

DWORD AsyncRequest::getCancelID()
{
	return cancelID;
}

frl::Bool AsyncRequest::isCancelled()
{
	return cancelled;
}

void AsyncRequest::isCancelled( Bool isCancelled_ )
{
	cancelled = isCancelled_;
}

void AsyncRequest::init( const std::list< OPCHANDLE > &handles_, const VARIANT *values_ )
{
	if( ! itemHVQTList.empty() )
		itemHVQTList.clear();

	if( handles_.empty() || values_ == NULL )
		FRL_THROW_S_CLASS( AsyncRequest::InvalidParameter );

	size_t i = 0;
	std::list< OPCHANDLE >::const_iterator end = handles_.end();
	for( std::list< OPCHANDLE >::const_iterator it = handles_.begin(); it != end; ++it, ++i )
	{
		ItemHVQT tmp;
		tmp.setHandle( (*it) );
		tmp.setValue( values_[i] );
		itemHVQTList.push_back( tmp );
	}
}

void AsyncRequest::init( const std::list< OPCHANDLE > &handles_ )
{
	if( ! itemHVQTList.empty() )
		itemHVQTList.clear();

	if( handles_.empty() )
		FRL_THROW_S_CLASS( AsyncRequest::InvalidParameter );
	
	std::list< OPCHANDLE >::const_iterator end = handles_.end();
	for( std::list< OPCHANDLE >::const_iterator it = handles_.begin(); it != end; ++it )
	{
		ItemHVQT tmp;
		tmp.setHandle( (*it) );
		itemHVQTList.push_back( tmp );
	}
}

const std::list< ItemHVQT >& AsyncRequest::getItemHVQTList() const
{
	return itemHVQTList;
}

size_t AsyncRequest::getCounts() const
{
	return itemHVQTList.size();
}

void AsyncRequest::removeHandle( OPCHANDLE handle )
{
	if( itemHVQTList.empty() )
		return;

	std::list< ItemHVQT >::iterator end = itemHVQTList.end();
	for( std::list< ItemHVQT >::iterator it = itemHVQTList.begin(); it != end; ++it )
	{
		if( (*it).getHandle() == handle )
		{
			itemHVQTList.erase( it );
			return;
		}
	}
}

DWORD AsyncRequest::getSource() const
{
	return source;
}

void AsyncRequest::setSource( DWORD source_ )
{
	source = source_;
}

DWORD AsyncRequest::getUniqueCancelID()
{
	static DWORD id = 0;
	return ++id;
}

AsyncRequest& AsyncRequest::operator=( const AsyncRequest &request )
{
	if( this == &request )
		return *this;
	
	AsyncRequest tmp( request );
	swap( tmp );
	return *this;
}

void AsyncRequest::swap( AsyncRequest &req )
{
	itemHVQTList.swap( req.itemHVQTList );
	std::swap( id, req.id );
	std::swap( cancelled, req.cancelled );
	std::swap( cancelID, req.cancelID );
	std::swap( source, req.source );
}

OPCHANDLE AsyncRequest::getGroupHandle()
{
	return groupHandle;
}

} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM == FRL_PLATFORM_WIN32
