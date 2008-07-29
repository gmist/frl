#ifndef frl_ios_h_stream_std_
#define frl_ios_h_stream_std_

#include "frl_platform.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ios>
#include "frl_types.h"

#if defined ( __linux__ ) || defined ( __linux )
	#include "stream_std/frl_streambuf.h"
#endif // __linux__

#if defined( FRL_PLATFORM_WIN32 )
	#include "stream_std\frl_streambuf.h"
#endif // WIN32

namespace frl{ namespace stream_std{

#if defined ( UNICODE ) || defined ( _UNICODE )
	typedef std::wstreampos Pos;
#else
	typedef std::streampos Pos;
#endif

} // namespace stream_std
} // FatRat Library

#endif // FRL_IOS_H_STREAM_STD_
