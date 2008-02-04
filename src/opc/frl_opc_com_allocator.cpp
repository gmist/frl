#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <ObjBase.h>
#include "opc\frl_opc_com_allocator.h"

namespace frl
{
	namespace opc
	{
		void* ComAllocator::operator new( size_t size )
		{
			return CoTaskMemAlloc( size);
		}

		void ComAllocator::operator delete( void* ptr )
		{
			if( ptr != NULL )
			{
				CoTaskMemFree( ptr );
				ptr = NULL;
			}
		}
	}
}

#endif // FRL_PLATFORM_WIN32
