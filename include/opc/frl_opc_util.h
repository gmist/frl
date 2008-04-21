#ifndef frl_opc_util_h_
#define frl_opc_util_h_
#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"
#include "frl_exception.h"

#if ! ( defined LOCALE_INVARIANT )
#define LOCALE_INVARIANT \
(MAKELCID(MAKELANGID(LANG_INVARIANT, SUBLANG_NEUTRAL), SORT_DEFAULT))
#endif


namespace frl
{
namespace opc
{
namespace util
{

OPCHANDLE getUniqueServerHandle();	
const FILETIME& getFileTime();
String getUniqueName();

char* duplicateString( const char *str );
wchar_t* duplicateString( const wchar_t *str );
char* duplicateString( const std::string &string );
wchar_t* duplicateString( const std::wstring &string );

HRESULT getErrorString(  HRESULT dwError, LCID dwLocale, LPWSTR **ppString );
Bool matchStringPattern( const String &str, const String& pattern, Bool caseSensitive = True );

const wchar_t* getPropertyDesc( DWORD propID );
VARTYPE getPropertyType( DWORD propID );

} // namespace util
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif /* frl_opc_util_h_ */
