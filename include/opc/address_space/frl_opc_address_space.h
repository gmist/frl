#ifndef frl_opc_address_space_h_
#define frl_opc_address_space_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <vector>
#include "opc/address_space/frl_opc_tag.h"
#include "frl_exception.h"

namespace frl
{
	namespace opc
	{
		namespace address_space
		{
			FRL_EXCEPTION_CLASS( NotFinalConstruct );
			FRL_EXCEPTION_CLASS( InvalidBranchName );
			FRL_EXCEPTION_CLASS( InvalidLeafName );

			class AddressSpace
			{
			private:
				String delimiter;
				Tag *rootTag;
				Tag *curPos;
				
			public:
				AddressSpace();

				~AddressSpace();

				const String& getDelimiter();

				void finalConstruct( const String &delimiter_ );

				void addBranch( const String &fullPath, Bool createPath = False );

				Tag* getBranch( const String& fullPath );

				Tag* getLeaf( const String& fullPath );

				Tag* getTag( const String &fullPath );

				void addLeaf( const String& fullPath, Bool createPath = False );

				String getCurPosPath(); 

				Bool isExistItem( const String &itemName );

				void goToRoot();

				Bool goUp();

				void goDown( const String &path );

				void goTo( const String &fullPath );

				Bool isBrowseRoot();

				void browseBranches( std::vector< String > &branches );

				void browseLeafs( std::vector< String > &leafs, DWORD accessFilter = 0 );

				Bool isExistBranch( const String &name );

				Bool isExistLeaf( const String &name );

				Bool isExistTag( const String &name );

				Bool isExistTag( OPCHANDLE handle );
			};
		} // namespace address_space
		extern address_space::AddressSpace opcAddressSpace;
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_address_space_h_*/
