#ifndef frl_opc_com_allocator_h_
#define frl_opc_com_allocator_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

namespace frl
{
	namespace opc
	{
		struct ComAllocator
		{
			void* operator new( size_t size );
			void operator delete( void* ptr );
		}; // class ComAllocator
	}
}

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_com_allocator_h_
