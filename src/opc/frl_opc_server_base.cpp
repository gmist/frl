#include "opc/frl_opc_server_base.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_group.h"

namespace frl
{
namespace opc
{

OPCServerBase::~OPCServerBase()
{
}

void OPCServerBase::removeGroupFromRequestList( OPCHANDLE group_handle )
{
	request_manager.removeGroupFromRequest( group_handle );
}

HRESULT OPCServerBase::setGroupName( const String &oldName, const String &newName )
{
	boost::mutex::scoped_lock guard( scopeGuard );
	try
	{
		group_manager.renameGroup( oldName, newName );
	}
	catch( GroupManager::IsExistGroup )
	{
		return OPC_E_DUPLICATENAME;
	}
	catch( GroupManager::NotExistGroup )
	{
		return E_INVALIDARG;
	}
	return S_OK;
}

frl::opc::GroupElem OPCServerBase::cloneGroup( String &name , String &to_name )
{
	return group_manager.cloneGroup( name, to_name );
}

frl::Bool OPCServerBase::asyncRequestCancel( DWORD id )
{	
	return ( request_manager.cancelRequest( id ) );
}

void OPCServerBase::removeItemFromRequestList( OPCHANDLE item_handle )
{
	request_manager.removeItemFromRequest( item_handle );
}

void OPCServerBase::addAsyncRequest( AsyncRequestListElem &request )
{
	request_manager.addRequest( request );
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
