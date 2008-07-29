#ifndef frl_os_win32_registry_key_h_
#define frl_os_win32_registry_key_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <vector>
#include "frl_string.h"
#include <boost/noncopyable.hpp>
#include "os/win32/registry/frl_registry_RootKey.h"
#include "os/win32/registry/frl_registry_RootKeys.h"

namespace frl{ namespace os{ namespace win32{ namespace registry{

class Key : private boost::noncopyable
{
private:
	frl::String name;
	frl::String path;
	RootKey rootKey;
	frl::Bool isOpenForWrite;
	HKEY hKey;
	HKEY open( DWORD premission );
	void recurseDeleteKey( frl::String name_ );
public:
	Key( frl::String name_, frl::String path_, RootKey rootKey_ = RootKeys::currentUser );
	Key( frl::String fullPath, RootKey rootKey_ = RootKeys::currentUser );
	Key();
	~Key();
	void reinit( frl::String name_, frl::String path_, RootKey rootKey_ = RootKeys::currentUser );
	void reinit( frl::String fullString, RootKey rootKey_ = RootKeys::currentUser );
	HKEY openForWrite();
	HKEY openForRead();
	HKEY openForRW();
	void deleteValue( frl::String valueName );
	void deleteKey( frl::Bool recursive = frl::False );
	HKEY create();
	void close();
	frl::Bool isExist();
	frl::Bool operator == ( const Key& rhs );
	int getNumSubkeys();
	int getNumSubvalues();
	Bool isExistValue( const String &name );
	void setStringValue( const String &name, const String &data );
	String getStringValue( const String &name );
	void setStringValue( const String &data );
	String getStringValue();
	void setDWORDValue( const String &name, DWORD data );
	DWORD getDWORDValue( const String &name );
	void setDWORDValue( DWORD data );
	DWORD getDWORDValue();
	void setMultiStringValue( const String &name, const std::vector< String > &data );
	std::vector< String > getMultiStringValue( const String &name );
	void setMultiStringValue( const std::vector< String > &data );
	std::vector< String > getMultiStringValue();
	void setBinaryValue( const String &name,  const std::vector< unsigned char > &data);
	std::vector< unsigned char > getBinaryValue( const String &name );
	void setBinaryValue( const std::vector< unsigned char > &data);
	std::vector< unsigned char > getBinaryValue();
	const RootKey& getRootKey();
};

} // namespace registry
} // namespace win32
} // namespace os
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_os_win32_registry_key_h_
