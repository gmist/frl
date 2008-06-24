#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <boost/thread/thread.hpp>
#include "opc/frl_opc_group_manager.h"
#include "opc/frl_opc_group.h"

namespace frl
{
namespace opc
{

GroupManager::GroupManager()
{
	updateThread = boost::thread(boost::bind( &GroupManager::updateGroups, this ) );
}

GroupManager::~GroupManager()
{
	stopUpdate.signal();
	updateThread.join();
}

void GroupManager::insert( GroupElem& group )
{
	handles_map.insert( std::pair< OPCHANDLE, GroupElem >( group->getServerHandle(), group ) );
	names_map.insert( std::pair< String, GroupElem >( group->getName(), group ) );
}

GroupElem GroupManager::addGroup( String &name )
{
	boost::mutex::scoped_lock lock( guard );
	FRL_EXCEPT_GUARD();
	if( names_map.find( name ) != names_map.end() )
		FRL_THROW_S_CLASS( IsExistGroup );
	GroupElem new_group( new Group( name ) );
	insert( new_group );
	return new_group;
}

bool GroupManager::removeGroup( String &name )
{
	boost::mutex::scoped_lock lock( guard );
	FRL_EXCEPT_GUARD();
	GroupElemNamesMap::iterator name_it = names_map.find( name );
	if( name_it == names_map.end() )
		FRL_THROW_S_CLASS( NotExistGroup );
	name_it->second->isDeleted( True );
	OPCHANDLE tmp_handle = name_it->second->getServerHandle();
	names_map.erase( name_it );
	
	GroupElemHandlesMap::iterator handle_it = handles_map.find( tmp_handle );
	if( handle_it == handles_map.end() )
		FRL_THROW_S_CLASS( NotExistGroup );
	Group *ptr = handle_it->second.get();
	ptr->AddRef();
	handles_map.erase( handle_it );
	return ptr->Release() == 0;
}

bool GroupManager::removeGroup( OPCHANDLE handle )
{
	boost::mutex::scoped_lock lock( guard );
	FRL_EXCEPT_GUARD();
	
	GroupElemHandlesMap::iterator handle_it = handles_map.find( handle );
	if( handle_it == handles_map.end() )
		FRL_THROW_S_CLASS( NotExistGroup );
	handle_it->second->isDeleted( True );
	String name = handle_it->second->getName();
	handles_map.erase( handle_it );
	
	GroupElemNamesMap::iterator name_it = names_map.find( name );
	if( name_it == names_map.end() )
		FRL_THROW_S_CLASS( NotExistGroup );
	Group *ptr = name_it->second.get();
	ptr->AddRef();
	names_map.erase( name_it );
	return ptr->Release() == 0;
}

frl::opc::GroupElem GroupManager::cloneGroup( String &name, String &to_name )
{
	boost::mutex::scoped_lock lock( guard );
	FRL_EXCEPT_GUARD();
	GroupElemNamesMap::iterator end = names_map.end();
	GroupElemNamesMap::iterator it = names_map.find( to_name );
	if( it != end )
		FRL_THROW_S_CLASS( IsExistGroup );

	it = names_map.find( name );
	if( it == end )
		FRL_THROW_S_CLASS( NotExistGroup );

	GroupElem new_group( it->second->clone() );
	new_group->setName( to_name );
	insert( new_group );
	return new_group;
}

void GroupManager::renameGroup( const String &name, const String &to_name )
{
	boost::mutex::scoped_lock lock( guard );
	FRL_EXCEPT_GUARD();
	GroupElemNamesMap::iterator end = names_map.end();
	GroupElemNamesMap::iterator it = names_map.find( to_name );
	if( it != end )
		FRL_THROW_S_CLASS( IsExistGroup );
	it = names_map.find( name );
	if( it == end )
		FRL_THROW_S_CLASS( NotExistGroup );
	GroupElem group = it->second;
	names_map.erase( it );
	group->setName( to_name );
	names_map.insert( std::pair< String, GroupElem >( name, group ) );
}

frl::opc::GroupElem GroupManager::getGroup( OPCHANDLE handle )
{
	boost::mutex::scoped_lock lock( guard );
	FRL_EXCEPT_GUARD();
	GroupElemHandlesMap::iterator it = handles_map.find( handle );
	if( it == handles_map.end() )
		FRL_THROW_S_CLASS( NotExistGroup );
	return it->second;
}

frl::opc::GroupElem GroupManager::getGroup( String &name )
{
	boost::mutex::scoped_lock lock( guard );
	FRL_EXCEPT_GUARD();
	GroupElemNamesMap::iterator it = names_map.find( name );
	if( it == names_map.end() )
		FRL_THROW_S_CLASS( NotExistGroup );
	return it->second;
}

std::vector< String > GroupManager::getNamesEnum()
{
	boost::mutex::scoped_lock lock( guard );
	FRL_EXCEPT_GUARD();
	size_t size = names_map.size();
	if( size == 0 )
		FRL_THROW_S_CLASS( NotExistGroup );
	std::vector< String > vec;
	vec.reserve( size );
	GroupElemNamesMap::iterator end = names_map.end();
	for(	GroupElemNamesMap::iterator it = names_map.begin();
			it != end;
			++it )
	{
		vec.push_back( it->second->getName() );
	}
	return vec;
}

std::vector< GroupElem > GroupManager::getGroupEnum()
{
	boost::mutex::scoped_lock lock( guard );
	FRL_EXCEPT_GUARD();
	size_t size = names_map.size();
	if( size == 0 )
		FRL_THROW_S_CLASS( NotExistGroup );
	std::vector< GroupElem > vec;
	vec.reserve( size );
	GroupElemNamesMap::iterator end = names_map.end();
	for(	GroupElemNamesMap::iterator it = names_map.begin();
			it != end;
			++it )
	{
		vec.push_back( it->second );
	}
	return vec;
}

size_t GroupManager::getGroupCount()
{
	return handles_map.size();
}

void GroupManager::updateGroups()
{
	while( ! stopUpdate.timedWait( 50 ) )
	{
		boost::mutex::scoped_lock lock( guard );
		boost::thread_group gr;
		FILETIME ftime;
		::GetSystemTimeAsFileTime( &ftime );
		ULONGLONG time = *(reinterpret_cast< ULONGLONG* >( &ftime ) );
		GroupElemNamesMap::iterator end = names_map.end();
		for(	GroupElemNamesMap::iterator it = names_map.begin();
			it != end;
			++it )
		{
			if( ( ( time - it->second->getLastUpdateTick() ) / 10000 ) >= it->second->getUpdateRate() )
			{
				gr.create_thread( boost::bind( &Group::onUpdateTimer, it->second.get() ) );
			}
		}
		gr.join_all();
	}
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
