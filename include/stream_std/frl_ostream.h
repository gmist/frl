#ifndef FRL_OSTREAM_H_STREAM_STD_
#define FRL_OSTREAM_H_STREAM_STD_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ostream>
#include "frl_types.h"

namespace frl
{
	namespace stream_std
	{
		// Преопределяем переменные
		typedef std::basic_ostream< frl::Char, std::char_traits< frl::Char > > Out;
	}// namespace stream_std
}// FatRat Library

#endif /* FRL_OSTREAM_H_STREAM_STD_ */
