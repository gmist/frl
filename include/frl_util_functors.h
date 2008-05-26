#ifndef frl_util_functors_h_
#define frl_util_functors_h_
#include <map>

namespace frl
{
namespace util_functors
{
template< typename T >
void DeAlloc( T* p )
{
	delete p;
	p = NULL;
}

template< typename T1, typename T2 >
void MapSecondDeAlloc( const std::pair< T1, T2 > &p )
{
	delete p.second;
}

}
}

#endif // frl_util_functors_h_
