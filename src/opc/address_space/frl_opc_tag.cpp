#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/address_space/frl_opc_tag.h"
#include "opc/frl_opc_util.h"

namespace frl
{
	namespace opc
	{
		namespace address_space
		{
			Tag::Tag()
			{
				is_Branch = False;
				parent = NULL;

				requestedDataType = VT_EMPTY;
				clientHandle = serverHandle = 0;
				accessRights = OPC_READABLE & OPC_WRITEABLE;
				active = True;
				quality = OPC_QUALITY_GOOD;
				scanRate = 0;

				SYSTEMTIME SystemTime;
				GetSystemTime( &SystemTime );
				SystemTimeToFileTime( &SystemTime, &timeStamp );
			}

			Tag::Tag( Bool is_Branch_, const String &delimiter_ )
			{
				if( delimiter_.empty() )
					delimiter = FRL_STR('.');
				else
					delimiter = delimiter_;

				parent = NULL;
				is_Branch = is_Branch_;
				if( is_Branch )
					value.setType( VT_ARRAY );
				else
					value.setType( VT_EMPTY );
				requestedDataType = VT_EMPTY;
				clientHandle = serverHandle = 0;
				accessRights = OPC_READABLE | OPC_WRITEABLE;
				active = True;
				quality = OPC_QUALITY_GOOD;
				scanRate = 0;

				SYSTEMTIME SystemTime;
				GetSystemTime( &SystemTime );
				SystemTimeToFileTime( &SystemTime, &timeStamp );
			}

			Tag::~Tag()
			{
				for( std::map< String, Tag* >::iterator it = tagsNameCache.begin(); it != tagsNameCache.end(); ++it )
					delete (*it).second;
				
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


			void Tag::setClientHandle( OPCHANDLE newHandle )
			{
				clientHandle = newHandle;
			}

			OPCHANDLE Tag::getClientHandle()
			{
				return clientHandle;
			}

			void Tag::setServerHandle( OPCHANDLE newHandle )
			{
				serverHandle = newHandle;
			}

			OPCHANDLE Tag::getServerHandle()
			{
				return serverHandle;
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

			void Tag::isActive( Bool actived )
			{
				active = actived;
			}

			frl::Bool Tag::isActive()
			{
				return active;
			}

			void Tag::isWriteable( Bool writeable )
			{
				if( writeable )
					accessRights |= OPC_WRITEABLE;
				else
					accessRights = OPC_READABLE;
			}

			frl::Bool Tag::isWriteable()
			{
				return ( accessRights & OPC_WRITEABLE ) == 1;
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
				newTag->setServerHandle( util::getUniqueServerHandle() );
				tagsNameCache.insert( std::pair< String, Tag* >( name, newTag ) );
				return newTag;
			}

			Tag* Tag::getBranch( const String &name )
			{
				std::map< String, Tag*>::iterator it = tagsNameCache.find( name );
				if( it == tagsNameCache.end() )
					FRL_THROW_S_CLASS( NotExistTag );
				if( ! (*it).second->isBranch() )
					FRL_THROW_S_CLASS( IsNotBranch );
				return (*it).second;
			}

			Tag* Tag::getLeaf( const String &name )
			{
				std::map< String, Tag*>::iterator it = tagsNameCache.find( name );
				if( it == tagsNameCache.end() )
					FRL_THROW_S_CLASS( NotExistTag );
				if( ! (*it).second->isLeaf() )
					FRL_THROW_S_CLASS( IsNotLeaf );
				return (*it).second;
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
				for( std::map< String, Tag* >::iterator it = tagsNameCache.begin(); it != tagsNameCache.end(); ++it )
				{
					if( (*it).second->isBranch() )
						branches.push_back( (*it).second->getShortID() );
				}
			}

			void Tag::browseLeafs( std::vector< String > &leafs, DWORD accessFilter )
			{
				for( std::map< String, Tag* >::iterator it = tagsNameCache.begin(); it != tagsNameCache.end(); ++it )
				{
					if( (*it).second->isLeaf() )
					{
						if( accessFilter != 0 )
						{
							if( ( accessFilter & (*it).second->getAccessRights() ) == 0 )
								continue;
						}
						leafs.push_back( (*it).second->getShortID() );
					}
				}
			}

			const ComVariant& Tag::read()
			{
				return value;
			}

			void Tag::write( const ComVariant &newVal )
			{
				if( ComVariant::isEqual( value, newVal ) )
					return;
				value = newVal;
				SYSTEMTIME SystemTime;
				GetSystemTime( &SystemTime );
				SystemTimeToFileTime( &SystemTime, &timeStamp );
			}

			const FILETIME& Tag::getTimeStamp()
			{
				return timeStamp;
			}

			void Tag::setQuality( DWORD quality_ )
			{
				quality = quality_;
			}

			DWORD Tag::getQuality()
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
		} // namespace address_space
	} // namespace opc
} // namespace FatRat Library

#endif
