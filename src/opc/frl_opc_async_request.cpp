#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_async_request.h"
#include "os/win32/com/frl_os_win32_com_variant.h"
#include "opc/frl_opc_group.h"

namespace frl{ namespace opc{

using namespace os::win32::com;

AsyncRequest::AsyncRequest(	GroupElem& group_,
											async_request::RequestType type_ )
	:	id( 0 ),
		cancelID( getUniqueCancelID() ),
		cancelled( False ),
		source( OPC_DS_DEVICE ),
		group( group_ ),
		type( type_ )
{
}

AsyncRequest::AsyncRequest(	GroupElem& group_, 
											async_request::RequestType type_,
											const std::list< OPCHANDLE > &handles_ )
	:	id( 0 ),
		cancelID( getUniqueCancelID() ),
		cancelled( False ),
		source( 0 ),
		group( group_ ),
		type( type_ )
{
	std::list< OPCHANDLE >::const_iterator end = handles_.end();
	for( std::list< OPCHANDLE >::const_iterator it = handles_.begin(); it != end; ++it )
	{
		ItemHVQT tmp;
		tmp.setHandle( (*it) );
		itemHVQTList.push_back( tmp );
	}
}

AsyncRequest::AsyncRequest(	GroupElem& group_,
											async_request::RequestType type_,
											const std::list< ItemHVQT >& itemsList )
	:	id( 0 ),
		cancelID( getUniqueCancelID() ),
		cancelled( False ),
		itemHVQTList( itemsList ),
		source( 0 ),
		group( group_ ),
		type( type_ )
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
	for( std::list< ItemHVQT >::iterator it = itemHVQTList.begin(); it != end; )
	{
		if( (*it).getHandle() == handle )
			itemHVQTList.erase( it++ );
		else
			++it;
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

GroupElem AsyncRequest::getGroup()
{
	return group;
}

frl::Bool AsyncRequest::isRead()
{
	return type == async_request::READ;
}

frl::Bool AsyncRequest::isWrite()
{
	return type == async_request::WRITE;
}

frl::Bool AsyncRequest::isUpdate()
{
	return type == async_request::UPDATE;
}
} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM == FRL_PLATFORM_WIN32
