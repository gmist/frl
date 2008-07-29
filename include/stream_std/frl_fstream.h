#ifndef frl_fstream_h_stream_std_
#define frl_fstream_h_stream_std_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
#include "frl_types.h"

namespace frl{ namespace stream_std{

// Переопределяем поток для ввода и вывода
typedef std::basic_fstream< frl::Char, std::char_traits< frl::Char > > FileStream;
typedef FileStream InOutFile;
// Переопределяем входной файловый поток
typedef std::basic_ifstream< frl::Char, std::char_traits< frl::Char > > InFile;
// Переопределяем выходной файловый поток
typedef std::basic_ofstream< frl::Char, std::char_traits< frl::Char > > OutFile;
typedef std::basic_filebuf< frl::Char, std::char_traits< frl::Char > > FileBuf;

} // namespace stream_std
} // FatRat Library

#endif // FRL_STREAM_H_STREAM_STD_
