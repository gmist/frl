#ifndef frl_opc_tag_h_
#define frl_opc_tag_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"
#include "frl_exception.h"
#include "os/win32/com/frl_os_win32_com_variant.h"
#include "frl_non_copyable.h"

namespace frl
{
namespace opc
{
namespace address_space
{

class Tag;
struct TagBrowseInfo
{
	String shortID;
	String fullID;
	Bool isLeaf;
	Tag* tagPtr;
};

class Tag : private NonCopyable
{
private:
	String id;
	String shortID;
	Bool is_Branch;
	VARTYPE requestedDataType;
	DWORD accessRights;
	String delimiter;
	Tag *parent;
	std::map< String, Tag* > tagsNameCache;
	os::win32::com::Variant value;
	WORD quality;
	FILETIME timeStamp;
	DWORD scanRate;
	Tag* addTag( const String &name, Bool is_Branch_ );
	Tag* getTag( const String &name );

public:

	FRL_EXCEPTION_CLASS( IsExistTag );
	FRL_EXCEPTION_CLASS( NotExistTag );
	FRL_EXCEPTION_CLASS( IsNotBranch );
	FRL_EXCEPTION_CLASS ( IsNotLeaf );

	Tag( Bool is_Branch_, const String &delimiter_ );

	~Tag();

	void setID( const String& newID );

	const String& getID();

	const String& getShortID();

	Bool isBranch();

	Bool isLeaf();

	void setRequestedDataType( VARTYPE newType );

	VARTYPE getReguestedDataType();

	void setCanonicalDataType( VARTYPE newType );

	VARTYPE getCanonicalDataType();

	void setAccessRights( DWORD newAccessRights );

	DWORD getAccessRights();

	void isWriteable( Bool writeable );

	Bool isWriteable();

	Tag* addLeaf( const String &name );

	Bool isExistTag( const String &name );

	Tag* addBranch( const String &name );

	Tag* getBranch( const String &name );

	Tag* getLeaf( const String &name );

	Tag* getParent();

	void setParent( Tag* parent_ );

	void browseBranches( std::vector< String > &branches );

	void browseLeafs( std::vector< String > &leaf, DWORD accessFilter = 0 );

	const os::win32::com::Variant& read();

	void write( const os::win32::com::Variant &newVal );

	void setQuality( WORD quality_ );

	WORD getQuality();

	const FILETIME& getTimeStamp();

	void setTimeStamp( FILETIME &ts );

	void setScanRate( DWORD scanRate_ );

	DWORD getScanRate();

	Bool isValidProperties( DWORD propertyID );

	Bool checkAccessRight( DWORD checkingAccessRight );

	std::vector< DWORD > getAvailableProperties();

	HRESULT getPropertyValue( DWORD propID, VARIANT &toValue );

	void browseLeafs( std::vector< TagBrowseInfo > &leafsArr );

	void browseBranches( std::vector< TagBrowseInfo > &branchesArr );

	void browse( std::vector< TagBrowseInfo > &arr );
};

} // namespace address_space
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif /* frl_opc_tag_h_*/
