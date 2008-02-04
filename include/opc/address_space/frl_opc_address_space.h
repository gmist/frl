#ifndef frl_opc_address_space_h_
#define frl_opc_address_space_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <vector>
#include <map>
#include "opc/address_space/frl_opc_tag.h"
#include "frl_exception.h"
#include "frl_non_copyable.h"

namespace frl
{
	namespace opc
	{
		namespace address_space
		{
			FRL_EXCEPTION_CLASS( NotFinalConstruct );
			FRL_EXCEPTION_CLASS( InvalidBranchName );
			FRL_EXCEPTION_CLASS( InvalidLeafName );

			class AddressSpace : private NonCopyable
			{
			private:
				String delimiter;
				Tag *rootTag;
				std::map< OPCHANDLE, Tag* > handleHash;
				std::map< String, Tag* > nameLeafHash;
				std::map< String, Tag* > nameBranchHash;

			public:
				AddressSpace();

				~AddressSpace();

				const String& getDelimiter();

				void finalConstruct( const String &delimiter_ );

				void addBranch( const String &fullPath, Bool createPath = False );

				Tag* getBranch( const String& fullPath );

				Tag* getLeaf( const String& fullPath );

				Tag* getTag( const String &fullPath );

				Tag* addLeaf( const String& fullPath, Bool createPath = False );

				Bool isExistBranch( const String &name );

				Bool isExistLeaf( const String &name );

				Bool isExistTag( const String &name );

				Bool isExistLeaf( OPCHANDLE handle );

				Tag* getLeaf( OPCHANDLE handle );

				Tag* getRootBranch();

				void getAllLeafs( std::vector< String > &namesList, DWORD accessFilter );
			};
		} // namespace address_space
		extern address_space::AddressSpace opcAddressSpace;
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_address_space_h_*/
