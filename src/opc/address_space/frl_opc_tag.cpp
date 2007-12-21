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

				requestedDataType = canonicalDataType = VT_EMPTY;
				clientHandle = serverHandle = 0;
				accessRights = OPC_READABLE & OPC_WRITEABLE;
				active = True;

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
					canonicalDataType = VT_ARRAY;
				else
					canonicalDataType = VT_EMPTY;
				requestedDataType = VT_EMPTY;
				clientHandle = serverHandle = 0;
				accessRights = OPC_READABLE | OPC_WRITEABLE;
				active = True;

				SYSTEMTIME SystemTime;
				GetSystemTime( &SystemTime );
				SystemTimeToFileTime( &SystemTime, &timeStamp );
			}

			Tag::~Tag()
			{
				if( tags.size() )
				{
					for( std::list< Tag* >::iterator it = tags.begin(); it != tags.end(); ++it )
					{
						Tag *tmp = (*it);
						delete tmp;
					}
					tags.erase( tags.begin(), tags.end() );
				}
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
				canonicalDataType = newType;
			}

			VARTYPE Tag::getCanonicalDataType()
			{
				return canonicalDataType;
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

			void Tag::addLeaf( const String &name )
			{
				FRL_EXCEPT_GUARD();
				addTag( name, False );
			}

			frl::Bool Tag::isExistTag( const String &name )
			{
				for( std::list< Tag* >::iterator it = tags.begin(); it != tags.end(); ++it )
				{
					if( (*it)->getID() == name )
						return True;
				}
				return False;
			}

			void Tag::addBranch( const String &name )
			{
				FRL_EXCEPT_GUARD();
				addTag( name, True );
			}

			void Tag::addTag( const String &name, Bool is_Branch_ )
			{
				FRL_EXCEPT_GUARD();
				if( isExistTag( name ) )
					FRL_THROW_S_CLASS( IsExistTag );
				Tag *newTag = new Tag( is_Branch_, delimiter );
				newTag->setParent( this );
				newTag->setID( name );
				newTag->setServerHandle( util::getUniqueServerHandle() );
				tags.push_back( newTag );
			}

			Tag* Tag::getBranch( const String &name )
			{
				for( std::list< Tag* >::iterator it = tags.begin(); it != tags.end(); ++it )
				{
					if( (*it)->getID() == name )
					{
						if( (*it)->isBranch() )
							return (*it);
						FRL_THROW_S_CLASS( IsNotBranch );
					}
				}
				FRL_THROW_S_CLASS( NotExistTag );
				return NULL;
			}

			Tag* Tag::getLeaf( const String &name )
			{
				for( std::list< Tag* >::iterator it = tags.begin(); it != tags.end(); ++it )
				{
					if( (*it)->getID() == name )
					{
						if( (*it)->isLeaf() )
							return (*it);
						FRL_THROW_S_CLASS( IsNotLeaf );
					}
				}
				FRL_THROW_S_CLASS( NotExistTag );
				return NULL;
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
				for( std::list< Tag* >::iterator it = tags.begin(); it != tags.end(); ++it )
				{
					if( (*it)->isBranch() )
						branches.push_back( (*it)->getShortID() );
				}
			}

			void Tag::browseLeafs( std::vector< String > &leafs, DWORD accessFilter )
			{
				for( std::list< Tag* >::iterator it = tags.begin(); it != tags.end(); ++it )
				{
					if( (*it)->isLeaf() )
					{
						if( accessFilter != 0 )
						{
							if( ( accessFilter & (*it)->getAccessRights() ) == 0 )
								continue;
						}
						leafs.push_back( (*it)->getShortID() );
					}
				}
			}

			const ComVariant& Tag::read()
			{
				return value;
			}

			void Tag::write( const ComVariant &newVal )
			{
				value = newVal;
				SYSTEMTIME SystemTime;
				GetSystemTime( &SystemTime );
				SystemTimeToFileTime( &SystemTime, &timeStamp );
			}

			ComVariant Tag::getTimeStamp()
			{
				return timeStamp;
			}
		} // namespace address_space
	} // namespace opc
} // namespace FatRat Library

#endif
