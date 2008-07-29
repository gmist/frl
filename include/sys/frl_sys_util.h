#ifndef frl_sys_util_h_
#define frl_sys_util_h_
#include "frl_types.h"

namespace frl{ namespace sys{ namespace util{

String getLastErrorDescription();
String getLastErrorDescription( WORD langID );
String getCodeErrorDescription( DWORD error );
String getCodeErrorDescription( WORD langID, DWORD error );

} // namespace util
} // namespace sys
} // namespace FatRat Library

#endif // frl_sys_util_h_
