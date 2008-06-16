#ifndef frl_opc_event_h_
#define frl_opc_event_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace frl
{
namespace opc
{

class Event : private boost::noncopyable
{
private:
	boost::condition cndt;
	boost::mutex mtx;
	volatile bool isEvent;
public:
	Event();
	~Event();
	void signal();
	void wait( void );
	bool timedWait( unsigned long time_ );
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_event_h_
