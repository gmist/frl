#ifndef frl_opc_tag_h_
#define frl_opc_tag_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"
#include "frl_exception.h"
#include "os/win32/com/frl_os_win32_com_variant.h"
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace frl{ namespace opc{ namespace address_space{

class Tag;
struct TagBrowseInfo
{
	String shortID;
	String fullID;
	Bool isLeaf;
	Tag* tagPtr;
};

class Tag : private boost::noncopyable
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

	Bool is_opc_change_subscr;
	boost::function< void() > opc_change;

	Bool is_opc_change_subscr_cb;
	boost::function< void( const address_space::Tag* const ) > opc_change_cb;

	Tag* addTag( const String &name, Bool is_Branch_ );
	Tag* getTag( const String &name );

	typedef std::map< String, Tag* >::value_type& map_element;
public:	

	FRL_EXCEPTION_CLASS( IsExistTag );
	FRL_EXCEPTION_CLASS( NotExistTag );
	FRL_EXCEPTION_CLASS( IsNotBranch );
	FRL_EXCEPTION_CLASS ( IsNotLeaf );
	FRL_EXCEPTION_CLASS ( AlreadySubscribe );

	Tag( Bool is_Branch_, const String &delimiter_ );

	~Tag();

	void setID( const String& newID );

	const String& getID() const;

	const String& getShortID() const;

	Bool isBranch() const;

	Bool isLeaf() const;

	void setRequestedDataType( VARTYPE newType );

	VARTYPE getReguestedDataType();

	void setCanonicalDataType( VARTYPE newType );

	VARTYPE getCanonicalDataType() const;

	void setAccessRights( DWORD newAccessRights );

	DWORD getAccessRights();

	void isWritable( Bool writeable );

	Bool isWritable() const;
	
	Bool isReadable() const;

	Tag* addLeaf( const String &name );

	Bool isExistTag( const String &name );

	Tag* addBranch( const String &name );

	Tag* getBranch( const String &name );

	Tag* getLeaf( const String &name );

	Tag* getParent();

	void setParent( Tag* parent_ );

	void browseBranches( std::vector< String > &branches );

	void browseLeafs( std::vector< String > &leaf, DWORD accessFilter = 0 );

	const os::win32::com::Variant& read() const;

	void writeFromOPC( const os::win32::com::Variant &newVal );

	void write( const os::win32::com::Variant &newVal );

	void setQuality( WORD quality_ );

	WORD getQuality() const;

	const FILETIME& getTimeStamp() const;

	void setTimeStamp( const FILETIME& ts );

	void setScanRate( DWORD scanRate_ );

	DWORD getScanRate();

	Bool isValidProperties( DWORD propertyID );

	Bool checkAccessRight( DWORD checkingAccessRight );

	std::vector< DWORD > getAvailableProperties() const;

	HRESULT getPropertyValue( DWORD propID, VARIANT &toValue );

	void browseLeafs( std::vector< TagBrowseInfo > &leafsArr );

	void browseBranches( std::vector< TagBrowseInfo > &branchesArr );

	void browse( std::vector< TagBrowseInfo > &arr );

	void subscribeToOpcChange( const boost::function< void() > &function_ );

	void subscribeToOpcChange( const boost::function< void( const address_space::Tag* const ) > &function_ );
};

} // namespace address_space
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_tag_h_
