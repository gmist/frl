#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/address_space/frl_opc_address_space.h"

namespace frl
{
	namespace opc
	{
		namespace address_space
		{
			AddressSpace::AddressSpace()
			{
				curPos = rootTag = NULL;
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
				curPos = rootTag;
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
					rootTag->addBranch( fullPath );
					return;
				}
				// last or first symbol in branch name == delimiter
				if( pos == fullPath.size()-1 || pos == 0 )
					FRL_THROW_S_CLASS( InvalidBranchName );
				String tmpPath = fullPath.substr( 0, pos );
				getBranch( tmpPath )->addBranch( fullPath );
			}

			Tag* AddressSpace::getBranch( const String& fullPath )
			{
				if( fullPath.empty() )
					return rootTag;
				size_t pos = fullPath.find( delimiter );
				if( pos == String::npos )
				{
					if( ! rootTag->isExistTag( fullPath ) )
						FRL_THROW_S_CLASS( NotExistTag );
					return rootTag->getBranch( fullPath );
				}
				String tmpBranch;
				String tmpPath = fullPath;
				Tag *brPos = rootTag;
				Tag *tmpBr;
				do
				{
					tmpBranch = tmpPath.substr( 0, pos );
					tmpBr = brPos->getBranch( tmpBranch );
					tmpPath = tmpPath.substr( pos+1, tmpPath.size()-1 );
					pos = tmpPath.find( delimiter );
					brPos = tmpBr;
				}
				while( pos != String::npos );
				return tmpBr->getBranch( fullPath );
			}

			void AddressSpace::addLeaf( const String &fullPath, Bool createPath )
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
						FRL_THROW_S_CLASS( IsExistTag );
					rootTag->addLeaf( fullPath );
					return;
				}
				String fullBranchName = fullPath.substr(0, pos );
								
				try
				{
					getBranch( fullBranchName )->addLeaf( fullPath );
					return;
				}
				catch( NotExistTag &ex )
				{
					ex.~NotExistTag();
					if( ! createPath )
						FRL_THROW_S_CLASS( NotExistTag );
				}

				pos = fullBranchName.find( delimiter );
				String tmpPath = fullBranchName;
				String tmpBranch;
				Tag* brPos = rootTag;
				Tag* tmpBr;
				do
				{
					tmpBranch = tmpPath.substr( 0, pos );
					try
					{
						tmpBr = brPos->getBranch( tmpBranch );
					}
					catch( NotExistTag &ex )
					{
						ex.~NotExistTag();
						brPos->addBranch( tmpBranch );
					}
					tmpPath = tmpPath.substr( pos+1, tmpPath.size()-1 );
					pos = tmpPath.find( delimiter );
					brPos = tmpBr;
				}
				while( pos != String::npos );
				getBranch( fullBranchName )->addLeaf( fullPath );
			}

			frl::Bool AddressSpace::isExistItem( const String &itemName )
			{
				return curPos->isExistTag( itemName);
			}

			void AddressSpace::goToRoot()
			{
				curPos = rootTag;
			}

			Bool AddressSpace::goUp()
			{
				if( curPos == rootTag )
					return False;
				curPos = curPos->getParent();
				return True;
			}

			void AddressSpace::goDown( const String &path )
			{
				curPos = curPos->getBranch( path );
			}

			void AddressSpace::goTo( const String &fullPath )
			{
				curPos = rootTag->getBranch( fullPath );
			}

			Tag* AddressSpace::getLeaf( const String& fullPath )
			{
				if( fullPath.empty() )
					return rootTag;
				size_t pos = fullPath.find( delimiter );
				if( pos == String::npos )
				{
					if( ! rootTag->isExistTag( fullPath ) )
						FRL_THROW_S_CLASS( NotExistTag );
					return rootTag->getLeaf( fullPath );
				}
				String tmpBranch;
				String tmpPath = fullPath;
				Tag *brPos = rootTag;
				Tag *tmpBr;
				do
				{
					tmpBranch = tmpPath.substr( 0, pos );
					tmpBr = brPos->getBranch( tmpBranch );
					tmpPath = tmpPath.substr( pos+1, tmpPath.size()-1 );
					pos = tmpPath.find( delimiter );
					brPos = tmpBr;
				}
				while( pos != String::npos );
				return tmpBr->getLeaf( fullPath );
			}

			frl::Bool AddressSpace::isBrowseRoot()
			{
				return curPos == rootTag;
			}

			void AddressSpace::browseBranches( std::vector< String > &branches )
			{
				if( branches.size() )
					branches.erase( branches.begin(), branches.end() );
				curPos->browseBranches( branches );
			}

			void AddressSpace::browseLeafs( std::vector< String > &leafs )
			{
				if( leafs.size() )
					leafs.erase( leafs.begin(), leafs.end() );
				curPos->browseLeafs( leafs );
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

			String AddressSpace::getCurPosPath()
			{
				return curPos->getID();
			}

			Tag* AddressSpace::getTag( const String &fullPath )
			{
				if( isExistLeaf( fullPath ) )
					return getLeaf( fullPath );
				if( isExistBranch( fullPath) )
					return getBranch( fullPath );
				FRL_THROW_S_CLASS( NotExistTag );
			}

			Bool AddressSpace::isExistTag( const String &fullPath )
			{
				if( isExistLeaf( fullPath ) )
					return True;
				if( isExistBranch( fullPath) )
					return True;
				return False;
			}

			Bool AddressSpace::isExistTag( OPCHANDLE serverHandle )
			{
				return False; // TODO
			}
		} // namespace address_space
		address_space::AddressSpace opcAddressSpace;
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
