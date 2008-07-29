#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <algorithm>
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/address_space/frl_opc_tag.h"
#include "opc/frl_opc_util.h"

namespace frl{ namespace opc{ namespace address_space{

namespace private_
{

template< typename T1, typename T2 >
void MapSecondDeAlloc( const std::pair< T1, T2 > &p )
{
	delete p.second;
}

} // namespace private_

Tag::Tag( Bool is_Branch_, const String &delimiter_ )
	:	is_Branch( is_Branch_ ),
		requestedDataType( VT_EMPTY ),
		accessRights( OPC_READABLE ),
		parent( NULL ),
		quality( OPC_QUALITY_GOOD ),
		scanRate( 0 )
{
	if( delimiter_.empty() )
		delimiter = FRL_STR('.');
	else
		delimiter = delimiter_;

	if( is_Branch )
		value.setType( VT_ARRAY );
	else
		value.setType( VT_EMPTY );
	::GetSystemTimeAsFileTime( &timeStamp );
}

Tag::~Tag()
{
	std::for_each( tagsNameCache.begin(), tagsNameCache.end(), private_::MapSecondDeAlloc< String, Tag* > );
}

void Tag::setID( const String& newID )
{
	id = newID;
	size_t pos = newID.rfind( delimiter );
	if( pos == String::npos )
		shortID = newID;
	else
		shortID = newID.substr( pos+1, newID.length() - 1 );
}

const String& Tag::getID()
{
	return id;
}

const String& Tag::getShortID()
{
	return shortID;
}

frl::Bool Tag::isBranch()
{
	return is_Branch;
}

frl::Bool Tag::isLeaf()
{
	return ! is_Branch;
}

void Tag::setRequestedDataType( VARTYPE newType )
{
	requestedDataType = newType;
}

VARTYPE Tag::getReguestedDataType()
{
	return requestedDataType;
}

void Tag::setCanonicalDataType( VARTYPE newType )
{
	value.setType( newType );
}

VARTYPE Tag::getCanonicalDataType()
{
	return value.getType();
}

void Tag::setAccessRights( DWORD newAccessRights )
{
	accessRights = newAccessRights;
}

DWORD Tag::getAccessRights()
{
	return accessRights;
}

void Tag::isWritable( Bool writeable )
{
	if( writeable )
		accessRights = OPC_READABLE | OPC_WRITEABLE;
	else
		accessRights = OPC_READABLE;
}

frl::Bool Tag::isWritable()
{
	return ( accessRights & OPC_WRITEABLE ) == OPC_WRITEABLE;
}

Tag* Tag::addLeaf( const String &name )
{
	FRL_EXCEPT_GUARD();
	return addTag( name, False );
}

frl::Bool Tag::isExistTag( const String &name )
{
	std::map< String, Tag*>::iterator it = tagsNameCache.find( name );
	if( it == tagsNameCache.end() )
		return False;
	return True;
}

Tag* Tag::addBranch( const String &name )
{
	FRL_EXCEPT_GUARD();
	return addTag( name, True );
}

Tag* Tag::addTag( const String &name, Bool is_Branch_ )
{
	FRL_EXCEPT_GUARD();
	if( isExistTag( name ) )
		FRL_THROW_S_CLASS( IsExistTag );
	Tag *newTag = new Tag( is_Branch_, delimiter );
	newTag->setParent( this );
	newTag->setID( name );
	tagsNameCache.insert( std::pair< String, Tag* >( name, newTag ) );
	return newTag;
}

Tag* Tag::getBranch( const String &name )
{
	Tag *tmp = getTag( name );
	if( ! tmp->isBranch() )
		FRL_THROW_S_CLASS( IsNotBranch );
	return tmp;
}

Tag* Tag::getLeaf( const String &name )
{
	Tag *tmp = getTag( name );
	if( ! tmp->isLeaf() )
		FRL_THROW_S_CLASS( IsNotLeaf );
	return tmp;
}

Tag* Tag::getParent()
{
	return parent;
}

void Tag::setParent( Tag* parent_ )
{
	parent = parent_;
}

void Tag::browseBranches( std::vector< String > &branches )
{
	branches.reserve( tagsNameCache.size() );
	std::map< String, Tag* >::iterator end = tagsNameCache.end();
	for( std::map< String, Tag* >::iterator it = tagsNameCache.begin(); it != end; ++it )
	{
		if( (*it).second->isBranch() )
			branches.push_back( (*it).second->getShortID() );
	}
}

void Tag::browseBranches( std::vector< TagBrowseInfo > &branchesArr )
{
	branchesArr.reserve( tagsNameCache.size() );
	std::map< String, Tag* >::iterator end = tagsNameCache.end();
	TagBrowseInfo tmp;
	for( std::map< String, Tag* >::iterator it = tagsNameCache.begin(); it != end; ++it )
	{
		if( (*it).second->isBranch() )
		{
			tmp.fullID = (*it).second->getID();
			tmp.shortID = (*it).second->getShortID();
			tmp.isLeaf = False;
			tmp.tagPtr = (*it).second;
			branchesArr.push_back( tmp );
		}
	}
}

void Tag::browseLeafs( std::vector< String > &leafs, DWORD accessFilter )
{
	leafs.reserve( tagsNameCache.size() );
	std::map< String, Tag* >::iterator end = tagsNameCache.end();
	for( std::map< String, Tag* >::iterator it = tagsNameCache.begin(); it != end; ++it )
	{
		if( (*it).second->isLeaf() )
		{
			if( accessFilter != 0 )
			{
				if( ! (*it).second->checkAccessRight( accessFilter ) )
					continue;
			}
			leafs.push_back( (*it).second->getShortID() );
		}
	}
}

void Tag::browseLeafs( std::vector< TagBrowseInfo > &leafsArr )
{
	leafsArr.reserve( tagsNameCache.size() );
	std::map< String, Tag* >::iterator end = tagsNameCache.end();
	TagBrowseInfo tmp;
	for( std::map< String, Tag* >::iterator it = tagsNameCache.begin(); it != end; ++it )
	{
		if( (*it).second->isLeaf() )
		{
			tmp.fullID = (*it).second->getID();
			tmp.shortID = (*it).second->getShortID();
			tmp.isLeaf = True;
			tmp.tagPtr = (*it).second;
			leafsArr.push_back( tmp );
		}
	}
}

const os::win32::com::Variant& Tag::read()
{
	return value;
}

void Tag::write( const os::win32::com::Variant &newVal )
{
	if( os::win32::com::Variant::isEqual( value, newVal ) )
		return;
	value = newVal;
	::GetSystemTimeAsFileTime( &timeStamp );
}

const FILETIME& Tag::getTimeStamp()
{
	return timeStamp;
}

void Tag::setQuality( WORD quality_ )
{
	quality = quality_;
}

WORD Tag::getQuality()
{
	return quality;
}

void Tag::setScanRate( DWORD scanRate_ )
{
	scanRate = scanRate_;
}

DWORD Tag::getScanRate()
{
	return scanRate;
}

Bool Tag::isValidProperties( DWORD propertyID )
{
	switch( propertyID )
	{
	case 0:
	case OPC_PROPERTY_VALUE:
	case OPC_PROPERTY_DATATYPE:  
	case OPC_PROPERTY_QUALITY:
	case OPC_PROPERTY_SCAN_RATE:
	case OPC_PROPERTY_TIMESTAMP:
	case OPC_PROPERTY_ACCESS_RIGHTS:
	{
	break;
	}	
	default:
		return False;
	}
	return True;
}

Bool Tag::checkAccessRight( DWORD checkingAccessRight )
{
	return ( accessRights & checkingAccessRight ) == checkingAccessRight;
}

std::vector< DWORD > Tag::getAvailableProperties()
{
	std::vector< DWORD > ret;
	ret.reserve( 6 );	
	ret.push_back( OPC_PROPERTY_DATATYPE );
	ret.push_back( OPC_PROPERTY_VALUE );
	ret.push_back( OPC_PROPERTY_QUALITY );
	ret.push_back( OPC_PROPERTY_TIMESTAMP );
	ret.push_back( OPC_PROPERTY_ACCESS_RIGHTS );
	ret.push_back( OPC_PROPERTY_SCAN_RATE );
	return ret;
}

HRESULT Tag::getPropertyValue( DWORD propID, VARIANT &toValue )
{
	using namespace os::win32::com;
	switch ( propID )
	{
		case OPC_PROPERTY_DATATYPE:
			return Variant::variantCopy( toValue, Variant( (short)value.getType() ) );

		case OPC_PROPERTY_VALUE:
			return Variant::variantCopy( toValue, read() );

		case OPC_PROPERTY_QUALITY:
			return Variant::variantCopy( toValue, Variant( (short)quality ) );

		case OPC_PROPERTY_TIMESTAMP:
			return Variant::variantCopy( toValue, Variant( timeStamp ) );

		case OPC_PROPERTY_ACCESS_RIGHTS:
			return Variant::variantCopy( toValue, Variant( (long)accessRights ) );

		case OPC_PROPERTY_SCAN_RATE:
			return Variant::variantCopy( toValue, Variant( (float)scanRate ) );
	}
	return OPC_E_INVALID_PID;
}

void Tag::browse( std::vector< TagBrowseInfo > &arr )
{
	arr.reserve( tagsNameCache.size() );
	std::map< String, Tag* >::iterator end = tagsNameCache.end();
	TagBrowseInfo tmp;
	for( std::map< String, Tag* >::iterator it = tagsNameCache.begin(); it != end; ++it )
	{
		tmp.fullID = (*it).second->getID();
		tmp.shortID = (*it).second->getShortID();
		if( (*it).second->isLeaf() )
			tmp.isLeaf = True;
		else
			tmp.isLeaf = False;
		tmp.tagPtr = (*it).second;
		arr.push_back( tmp );
	}
}

void Tag::setTimeStamp( const FILETIME& ts )
{
	timeStamp = ts;
}

Tag* Tag::getTag( const String &name )
{
	std::map< String, Tag*>::iterator it = tagsNameCache.find( name );
	if( it == tagsNameCache.end() )
		FRL_THROW_S_CLASS( NotExistTag );
	return (*it).second;
}

frl::Bool Tag::isReadable()
{
	return ( accessRights & OPC_READABLE ) == OPC_READABLE;	
}

} // namespace address_space
} // namespace opc
} // FatRat Library

#endif
