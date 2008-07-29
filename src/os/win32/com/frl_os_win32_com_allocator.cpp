#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <ObjBase.h>
#include "os/win32/com/frl_os_win32_com_allocator.h"

namespace frl{ namespace os{ namespace win32{ namespace com{

void freeMemory( void *ptr )
{
	CoTaskMemFree( ptr );
	ptr = NULL;
}

Bool isValidType( VARTYPE vtType )
{
	if (vtType & VT_ARRAY)
	{
		if (((vtType & VT_TYPEMASK) | VT_ARRAY) != vtType)
		{
			return False;
		}
	}

	switch (vtType & VT_TYPEMASK)
	{
	case VT_EMPTY:
	case VT_I1:
	case VT_UI1:
	case VT_I2:
	case VT_UI2:
	case VT_I4:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_R4:
	case VT_R8:
	case VT_CY:
	case VT_BOOL:
	case VT_DATE:
	case VT_BSTR:
	case VT_VARIANT:
		{
			return True;
		}
	}
	return False;
}

void* Allocator::operator new( size_t size )
{
	return CoTaskMemAlloc( size);
}

void Allocator::operator delete( void* ptr )
{
	if( ptr != NULL )
	{
		CoTaskMemFree( ptr );
		ptr = NULL;
	}
}

} // namespace com
} // namespace win32
} // namespace os
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
