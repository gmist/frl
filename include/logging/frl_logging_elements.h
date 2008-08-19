#ifndef frl_logging_elements_h_
#define frl_logging_elements_h_
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "frl_types.h"
#include "time/frl_time_sys_time.h"
#include "frl_lexical_cast.h"

namespace frl{ namespace logging{

enum Level
{
	LEVEL_TRACE = 0,
	LEVEL_MSG = 1,
	LEVEL_WARNING = 2,
	LEVEL_ERROR = 3,
	LEVEL_INFO = 4,
	LEVEL_NONE = 100
};

struct LogParameter
{
	frl::logging::Level level;
	ULong thread_id;
	String function;
	String file;
	ULong line;
	String msg;
};

class ILogElement
{
public:
	virtual String proccess(  const LogParameter &param ) = 0;
};


typedef boost::shared_ptr< frl::logging::ILogElement > LogElement;
typedef std::list< LogElement > LogElementList;

class LexicalElement : public ILogElement
{
public:
	String text;
public:
	LexicalElement( String msg );
	virtual ~LexicalElement();
	virtual String proccess(  const LogParameter &param );
};

class DateElement : public ILogElement
{
private:
	time::SysTime sysTime;
public:
	virtual ~DateElement();
	virtual String proccess(  const LogParameter &param );
};

class TimeElement : public ILogElement
{
private:
	time::SysTime sysTime;
public:
	virtual ~TimeElement();
	virtual String proccess(  const LogParameter &param );
};

class LevelElement : public ILogElement
{
public:
	virtual ~LevelElement();
	virtual String proccess(  const LogParameter &param );
};

class MsgElement : public ILogElement
{
public:
	virtual ~MsgElement();
	virtual String proccess(  const LogParameter &param );
};

class EndlElement : public ILogElement
{
public:
	virtual ~EndlElement();
	virtual String proccess(  const LogParameter &param );
};

class FileElement : public ILogElement
{
public:
	virtual ~FileElement();
	virtual String proccess( const LogParameter &param );
};

class LineElement : public ILogElement
{
public:
	virtual ~LineElement();
	virtual String proccess( const LogParameter &param );
};

class FunctionElement : public ILogElement
{
public:
	virtual ~FunctionElement();
	virtual String proccess( const LogParameter &param );
};

class ThreadIDElement : public ILogElement
{
public:
	virtual ~ThreadIDElement();
	virtual String proccess( const LogParameter &param );
};

template < class LHV, class RHV >
inline
LogElementList operator << ( const LHV &lhv, const RHV &rhv )
{
	LogElementList tmp;
	tmp.push_back( boost::make_shared< LHV >()	 );
	tmp.push_back( boost::make_shared< RHV >() );
	return tmp;
}

// Разбираемся со строками
inline
LogElementList operator << ( const String &lhv, const String &rhv )
{
	LogElementList tmp;
	tmp.push_back( boost::make_shared< LexicalElement >( lhv ) );
	tmp.push_back( boost::make_shared< LexicalElement >( rhv ) );
	return tmp;
}

inline
LogElementList operator << ( const String &lhv, LogElementList rhv )
{
	rhv.push_back( boost::make_shared< LexicalElement>( lhv ) );
	return rhv;
}

inline
LogElementList operator << ( LogElementList lhv, const String &rhv )
{
	lhv.push_back( boost::make_shared< LexicalElement >( rhv ) );
	return lhv;
}

template< class RHV >
inline
LogElementList operator << ( const String &lhv, const RHV &rhv )
{
	LogElementList tmp;
	tmp.push_back( boost::make_shared< LexicalElement >( lhv ) );
	tmp.push_back( boost::make_shared< RHV >() );
	return tmp;
}

template< class LHV >
inline
LogElementList operator << ( const LHV &lhv, const String &rhv )
{
	LogElementList tmp;
	tmp.push_back( boost::make_shared< LHV >() );
	tmp.push_back( boost::make_shared< LexicalElement >( rhv ) );
	return tmp;
}

// Разбираемся с остальными типами
template< class LHV >
inline
LogElementList operator << ( const LHV &lhv, LogElementList rhv )
{
	rhv.push_back( boost::make_shared< LHV >() );
	return rhv;
}

template< class RHV >
inline
LogElementList operator << ( LogElementList lhv, const RHV &rhv )
{
	lhv.push_back( boost::make_shared< RHV >() );
	return lhv;
}

} // namespace logging
} // FatRat Library

#endif // frl_logging_elements_h_
