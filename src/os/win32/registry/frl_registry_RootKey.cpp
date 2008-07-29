#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "os/win32/registry/frl_registry_RootKey.h"

#if ! defined HKEY_PERFORMANCE_NLSTEXT
#define HKEY_PERFORMANCE_NLSTEXT    (( HKEY ) (ULONG_PTR)((LONG)0x80000060) )
#endif

#if ! defined HKEY_PERFORMANCE_TEXT
#define HKEY_PERFORMANCE_TEXT       (( HKEY ) (ULONG_PTR)((LONG)0x80000050) )
#endif

namespace frl{ namespace os{ namespace win32{ namespace registry{

RootKey::RootKey()
	:	name( FRL_STR("HKEY_CURRENT_USER") ),
		value( HKEY_CURRENT_USER )
{					
}

RootKey::RootKey( frl::String newName )
	: name( newName ), value( NULL )
{
	if ( name == FRL_STR("HKEY_CLASSES_ROOT") )
	{
		value = HKEY_CLASSES_ROOT;
		return;
	}

	if ( name == FRL_STR("HKEY_CURRENT_USER") )
	{
		value = HKEY_CURRENT_USER;
		return;
	}

	if ( name == FRL_STR("HKEY_CURRENT_CONFIG") )
	{
		value = HKEY_CURRENT_CONFIG;
		return;
	}

	if ( name == FRL_STR("HKEY_DYN_DATA") )
	{
		value = HKEY_DYN_DATA;
		return;
	}

	if ( name == FRL_STR("HKEY_LOCAL_MACHINE") )
	{
		value = HKEY_LOCAL_MACHINE;
		return;
	}

	if ( name == FRL_STR("HKEY_PERFORMANCE_DATA") )
	{
		value = HKEY_PERFORMANCE_DATA;
		return;
	}

	if ( name == FRL_STR("HKEY_PERFORMANCE_NLSTEXT") )
	{
		value = HKEY_PERFORMANCE_NLSTEXT;
		return;
	}

	if ( name == FRL_STR("HKEY_PERFORMANCE_TEXT") )
	{
		value = HKEY_PERFORMANCE_TEXT;
		return;
	}

	if ( name == FRL_STR("HKEY_USERS") )
	{
		value = HKEY_USERS;
		return;
	}
} // RootKey( frl::String )

HKEY RootKey::getValue()
{
	return value;
}

frl::String RootKey::toString()
{
	return name;
}

RootKey::RootKey( const RootKey &rval )
{
	name = rval.name;
	value = rval.value;
}

RootKey& RootKey::operator = ( const RootKey &rval )
{
	if ( &rval == this )
	{
		return *this;
	}
	name = rval.name;
	value = rval.value;
	return *this;
}

frl::Bool RootKey::operator ==( const RootKey &rhs)
{
	return name == rhs.name;
}

} // namespace registry
} // namespace win32
} // namespace os
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
