#ifndef frl_streambuf_h_stream_std_
#define frl_streambuf_h_stream_std_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <streambuf>
#include "frl_types.h"

namespace frl{ namespace stream_std{

typedef std::basic_streambuf< frl::Char, std::char_traits< frl::Char > > Buf;

} // namespace stream_std
} // FatRat Library

#endif // frl_streambuf_h_stream_std_
