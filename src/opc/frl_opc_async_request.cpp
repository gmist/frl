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
		values( NULL ),
		source( OPC_DS_DEVICE ),
		groupHandle( groupHandle_ )
{
}

AsyncRequest::AsyncRequest( OPCHANDLE groupHandle_, const std::list< OPCHANDLE > &handles_ )
	:	id( 0 ),
		cancelID( getUniqueCancelID() ),
		cancelled( False ),
		handles( handles_ ),
		values( NULL ),
		source( 0 ),
		groupHandle( groupHandle_ )
{

}

AsyncRequest::AsyncRequest( const AsyncRequest &request )
	:	id( request.id ),
		cancelID( request.cancelID ),
		cancelled( request.cancelled ),
		handles( request.handles ),
		groupHandle( request.groupHandle ),
		values( request.values )
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
	if( ! values.empty() )
		values.clear();

	handles = handles_;
	size_t counts = handles.size();
	if( counts == 0 )
		FRL_THROW_S_CLASS( AsyncRequest::InvalidParameter );

	values.reserve( counts );
	for( DWORD i = 0; i < counts; ++i )
	{
		values.push_back( values_[i] );
	}
}

void AsyncRequest::init( const std::list< OPCHANDLE > &handles_ )
{
	if( ! values.empty() )
		values.clear();
	handles = handles_;
}

const std::list<OPCHANDLE>& AsyncRequest::getHandles() const
{
	return handles;
}

size_t AsyncRequest::getCounts() const
{
	return handles.size();
}

const std::vector< os::win32::com::Variant >&  AsyncRequest::getValues() const
{
	return values;
}

void AsyncRequest::removeHandle( OPCHANDLE handle )
{
	size_t originalSize = handles.size();
	if( originalSize == 0 )
		return;

	size_t posList = 0;
	std::list< OPCHANDLE >::iterator end = handles.end();
	for( std::list< OPCHANDLE >::iterator it = handles.begin(); it != end; ++it )
	{
		if( (*it) == handle )
		{
			handles.erase( it );
			break;
		}
		++posList;
	}

	if( ! originalSize )
		return;

	size_t size = handles.size();
	if( size == 0 )
	{
		values.clear();
		return;
	}
	if( size != originalSize )
	{
		std::vector< os::win32::com::Variant > tmpList;
		tmpList.reserve( size );
		for( size_t i = 0; i < originalSize; ++i )
		{
			if( i != posList )
			{
				tmpList.push_back( values[ i ] );
			}
		}
		values.swap( tmpList );
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
	handles.swap( req.handles );
	std::swap( id, req.id );
	std::swap( cancelled, req.cancelled );
	std::swap( cancelID, req.cancelID );
	std::swap( source, req.source );
	values.swap( req.values );
}

OPCHANDLE AsyncRequest::getGroupHandle()
{
	return groupHandle;
}

} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM == FRL_PLATFORM_WIN32