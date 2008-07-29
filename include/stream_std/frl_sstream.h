#ifndef frl_sstream_h_stream_std_
#define frl_sstream_h_stream_std_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sstream>
#include "frl_types.h"

namespace frl{ namespace stream_std{

// Переопределяем переменные
// stringstream
typedef std::basic_stringstream< frl::Char > StringStream;
// stringbuf
typedef std::basic_stringbuf< frl::Char > StringBuf;
// ostringstream
typedef std::basic_ostringstream< frl::Char > OutString;
// istringstream
typedef std::basic_istringstream< frl::Char > InString;

} // namespace stream_std
} // FatRat Library

#endif // frl_sstream_h_stream_std_
