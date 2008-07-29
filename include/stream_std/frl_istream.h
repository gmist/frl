#ifndef frl_istream_h_stream_std_
#define frl_istream_h_stream_std_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <istream>
#include "frl_types.h"

namespace frl{ namespace stream_std{

// Преопределяем переменные
typedef std::basic_iostream< frl::Char, std::char_traits< frl::Char > > InOut;
typedef std::basic_istream< frl::Char, std::char_traits< frl::Char > > In;

} // namespace stream_std
}// FatRat Library

#endif // frl_istream_h_stream_std_
