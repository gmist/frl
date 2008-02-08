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

			const String& AddressSpace::getDelimiter()
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

			void AddressSpace::addBranch( const String &fullPath, Bool createPath /*= False */ )
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
					nameBranchHash.insert( std::pair< String, Tag*>( fullPath, added ) );
					return;
				}
				// last or first symbol in branch name == delimiter
				if( pos == fullPath.size()-1 || pos == 0 )
					FRL_THROW_S_CLASS( InvalidBranchName );
				String tmpPath = fullPath.substr( 0, pos );
				Tag *added = getBranch( tmpPath )->addBranch( fullPath );
				nameBranchHash.insert( std::pair< String, Tag*>( fullPath, added ) );
			}

			Tag* AddressSpace::getBranch( const String& fullPath )
			{
				if( fullPath.empty() )
					return rootTag;
				std::map< String, Tag*>::iterator it = nameBranchHash.find( fullPath );
				if( it == nameBranchHash.end() )
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
					OPCHANDLE handle = added->getServerHandle();
					handleHash.insert( std::pair< OPCHANDLE, Tag* > ( handle, added ) );
					nameLeafHash.insert( std::pair< String, Tag*>( fullPath, added ) );
					return added;
				}
				String fullBranchName = fullPath.substr(0, pos );

				try
				{
					getBranch( fullBranchName )->addLeaf( fullPath );
					Tag *added = getBranch( fullBranchName )->getLeaf( fullPath );
					OPCHANDLE handle = added->getServerHandle();
					handleHash.insert( std::pair< OPCHANDLE, Tag* > ( handle, added ) );
					nameLeafHash.insert( std::pair< String, Tag*>( fullPath, added ) );
					return added;
				}
				catch( Tag::NotExistTag &ex )
				{
					ex.~NotExistTag();
					if( ! createPath )
						FRL_THROW_S_CLASS( Tag::NotExistTag );
				}
				return NULL;
			}

			Tag* AddressSpace::getLeaf( const String& fullPath )
			{
				if( fullPath.empty() )
					FRL_THROW_S_CLASS( InvalidLeafName );
				std::map< String, Tag*>::iterator it = nameLeafHash.find( fullPath );
				if( it == nameLeafHash.end() )
					FRL_THROW_S_CLASS( Tag::NotExistTag );
				return (*it).second;
			}

			Tag* AddressSpace::getLeaf( OPCHANDLE handle )
			{
				std::map< OPCHANDLE, Tag* >::iterator it = handleHash.find( handle );
				if( it == handleHash.end() )
					FRL_THROW_S_CLASS( Tag::NotExistTag );
				return (*it).second;
			}

			frl::Bool AddressSpace::isExistBranch( const String &name )
			{
				try
				{
					getBranch( name );
				}
				catch( frl::Exception &ex )
				{
					ex.~Exception();
					return False;
				}
				return True;
			}

			frl::Bool AddressSpace::isExistLeaf( const String &name )
			{
				try
				{
					getLeaf( name );
				}
				catch( frl::Exception &ex )
				{
					ex.~Exception();
					return False;
				}
				return True;
			}

			Tag* AddressSpace::getTag( const String &fullPath )
			{
				try
				{
					return getLeaf( fullPath );
				}
				catch( Tag::NotExistTag &ex )
				{
					ex.~NotExistTag();
				}
				return getBranch( fullPath );
			}

			Bool AddressSpace::isExistTag( const String &fullPath )
			{
				if( isExistLeaf( fullPath ) )
					return True;
				if( isExistBranch( fullPath) )
					return True;
				return False;
			}

			Bool AddressSpace::isExistLeaf( OPCHANDLE serverHandle )
			{
				std::map< OPCHANDLE, Tag* >::iterator it = handleHash.find( serverHandle );
				if( it != handleHash.end() )
					return True;
				return False;
			}

			Tag* AddressSpace::getRootBranch()
			{
				return rootTag;
			}

			void AddressSpace::getAllLeafs( std::vector< String > &namesList, DWORD accessFilter )
			{
				if ( namesList.size() )
					namesList.clear();
				for(	std::map< String, Tag* >::iterator it = nameLeafHash.begin();
						it != nameLeafHash.end(); ++it )
				{
					if( accessFilter != 0 )
					{
						if( ( accessFilter & (*it).second->getAccessRights() ) == 0 )
							continue;
					}
					namesList.push_back( (*it).first );
				}
			}

			frl::Bool AddressSpace::isInit()
			{
				return init;
			}
		} // namespace address_space
		address_space::AddressSpace opcAddressSpace;
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
