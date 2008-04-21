#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/address_space/frl_opc_address_space.h"

namespace frl
{
namespace opc
{
namespace address_space
{
AddressSpace::AddressSpace() : rootTag( NULL ), init( False )
{
	rootTag = NULL;
}

AddressSpace::~AddressSpace()
{
	delete rootTag;
}

const String& AddressSpace::getDelimiter() const
{
	return delimiter;
}

void AddressSpace::finalConstruct( const String &delimiter_ )
{
	if( delimiter_.empty() || delimiter.size()>1 )
		delimiter = FRL_STR('.');
	else
		delimiter = delimiter_;
	rootTag = new Tag( True, delimiter );
	init = True;
}

void AddressSpace::addBranch( const String &fullPath )
{
	FRL_EXCEPT_GUARD();
	if( rootTag == NULL )
		FRL_THROW_S_CLASS( NotFinalConstruct );
	if( fullPath.empty() )
		FRL_THROW_S_CLASS( InvalidBranchName );
	size_t pos = fullPath.rfind( delimiter + delimiter );
	if ( pos != String::npos )
		FRL_THROW_S_CLASS( InvalidBranchName );
	pos = fullPath.rfind( delimiter );
	if( pos == String::npos )
	{
		Tag *added = rootTag->addBranch( fullPath );
		nameBranchCache.insert( std::pair< String, Tag*>( fullPath, added ) );
		return;
	}
	// last or first symbol in branch name == delimiter
	if( pos == fullPath.size()-1 || pos == 0 )
		FRL_THROW_S_CLASS( InvalidBranchName );
	String tmpPath = fullPath.substr( 0, pos );
	Tag *added = getBranch( tmpPath )->addBranch( fullPath );
	nameBranchCache.insert( std::pair< String, Tag*>( fullPath, added ) );
}

Tag* AddressSpace::getBranch( const String& fullPath )
{
	if( fullPath.empty() )
		return rootTag;
	std::map< String, Tag*>::iterator it = nameBranchCache.find( fullPath );
	if( it == nameBranchCache.end() )
		FRL_THROW_S_CLASS( Tag::NotExistTag );
	return (*it).second;
}

Tag* AddressSpace::addLeaf( const String &fullPath, Bool createPath )
{
	FRL_EXCEPT_GUARD();
	if( fullPath.empty() )
		FRL_THROW_S_CLASS( InvalidLeafName );
	size_t pos = fullPath.rfind( delimiter + delimiter );
	if ( pos != String::npos )
		FRL_THROW_S_CLASS( InvalidLeafName );
	pos = fullPath.rfind( delimiter );
	if( pos == 0 || pos == fullPath.length()-1 )
		FRL_THROW_S_CLASS( InvalidLeafName );
	if( pos == String::npos )
	{
		if ( rootTag->isExistTag( fullPath ) )
			FRL_THROW_S_CLASS( Tag::IsExistTag );
		rootTag->addLeaf( fullPath );
		Tag *added = rootTag->getLeaf( fullPath );
		nameLeafCache.insert( std::pair< String, Tag*>( fullPath, added ) );
		return added;
	}
	String fullBranchName = fullPath.substr(0, pos );

	try
	{
		getBranch( fullBranchName )->addLeaf( fullPath );
		Tag *added = getBranch( fullBranchName )->getLeaf( fullPath );
		nameLeafCache.insert( std::pair< String, Tag*>( fullPath, added ) );
		return added;
	}
	catch( Tag::NotExistTag& )
	{
		if( ! createPath )
			FRL_THROW_S_CLASS( Tag::NotExistTag );
	}
	return NULL;
}

Tag* AddressSpace::getLeaf( const String& fullPath )
{
	if( fullPath.empty() )
		FRL_THROW_S_CLASS( InvalidLeafName );
	std::map< String, Tag*>::iterator it = nameLeafCache.find( fullPath );
	if( it == nameLeafCache.end() )
		FRL_THROW_S_CLASS( Tag::NotExistTag );
	return (*it).second;
}

frl::Bool AddressSpace::isExistBranch( const String &name ) const
{
	if( name.empty() )
		return True; // root branch
	if( nameBranchCache.find( name ) == nameBranchCache.end() )
		return False;
	return True;
}

frl::Bool AddressSpace::isExistLeaf( const String &name ) const
{
	if( name.empty() )
		return False;
	if( nameLeafCache.find( name ) == nameLeafCache.end() )
		return False;
	return True;
}

Tag* AddressSpace::getTag( const String &fullPath )
{
	try
	{
		return getLeaf( fullPath );
	}
	catch( frl::Exception& ){}
	return getBranch( fullPath );
}

Bool AddressSpace::isExistTag( const String &fullPath ) const
{
	if( isExistLeaf( fullPath ) || isExistBranch( fullPath) )
		return True;
	return False;
}

Tag* AddressSpace::getRootBranch()
{
	return rootTag;
}

void AddressSpace::getAllLeafs( std::vector< String > &namesList, DWORD accessFilter ) const
{
	if ( namesList.size() )
		namesList.clear();
	namesList.reserve( nameLeafCache.size() );
	std::map< String, Tag* >::const_iterator end = nameLeafCache.end();
	for(	std::map< String, Tag* >::const_iterator it = nameLeafCache.begin();
			it != end;
			++it )
	{
		if( accessFilter != 0 )
		{
			if( !(*it).second->checkAccessRight( accessFilter ) )
				continue;
		}
		namesList.push_back( (*it).first );
	}
}

frl::Bool AddressSpace::isInit() const
{
	return init;
}
} // namespace address_space

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
