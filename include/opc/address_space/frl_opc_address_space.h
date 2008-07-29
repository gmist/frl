#ifndef frl_opc_address_space_h_
#define frl_opc_address_space_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <vector>
#include <map>
#include "opc/address_space/frl_opc_tag.h"
#include "frl_exception.h"
#include <boost/noncopyable.hpp>
#include "frl_singleton.h"

namespace frl{ namespace opc{ namespace address_space{

class AddressSpace : private boost::noncopyable
{
private:
	String delimiter;
	Tag *rootTag;
	Bool init;
	std::map< String, Tag* > nameLeafCache;
	std::map< String, Tag* > nameBranchCache;

public:

	FRL_EXCEPTION_CLASS( NotFinalConstruct );
	FRL_EXCEPTION_CLASS( InvalidBranchName );
	FRL_EXCEPTION_CLASS( InvalidLeafName );

	AddressSpace();

	~AddressSpace();

	const String& getDelimiter() const;

	void finalConstruct( const String &delimiter_ );

	void addBranch( const String &fullPath );

	Tag* getBranch( const String& fullPath );

	Tag* getLeaf( const String& fullPath );

	Tag* getTag( const String &fullPath );

	Tag* addLeaf( const String& fullPath, Bool createPath = False );

	Bool isExistBranch( const String &name ) const;

	Bool isExistLeaf( const String &name ) const;

	Bool isExistTag( const String &name ) const;

	Tag* getRootBranch();

	void getAllLeafs( std::vector< String > &namesList, DWORD accessFilter ) const;

	Bool isInit() const;
};

} // namespace address_space

typedef SingletonMeyers< address_space::AddressSpace > opcAddressSpace;

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_address_space_h_
