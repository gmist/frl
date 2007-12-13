#ifndef FRL_STREAMBUF_H_STREAM_STD_
#define FRL_STREAMBUF_H_STREAM_STD_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <streambuf>
#include "frl_types.h"

namespace frl
{
	namespace stream_std
	{
		typedef std::basic_streambuf< frl::Char, std::char_traits< frl::Char > > Buf;
	}// namespace stream_std
}// namespace FatRat Library

#endif /* FRL_STREAMBUF_H_STREAM_STD_ */
