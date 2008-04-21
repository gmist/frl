#ifndef frl_logging_elements_h_
#define frl_logging_elements_h_
#include <list>
#include "frl_types.h"
#include "time/frl_time_sys_time.h"
#include "frl_smart_ptr.h"
#include "frl_lexical_cast.h"

namespace frl
{
namespace logging
{

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

typedef std::list< frl::SmartPtr< frl::logging::ILogElement > > ListLogElements;

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
ListLogElements operator << ( const LHV &lhv, const RHV &rhv )
{
	ListLogElements tmp;
	SmartPtr< LHV > lhvTmp( new LHV() );
	SmartPtr< RHV > rhvTmp( new RHV() );
	tmp.push_back( (const SmartPtr< ILogElement >&) lhvTmp );
	tmp.push_back( (const SmartPtr< ILogElement >&) rhvTmp );
	return tmp;
}

// Разбираемся со строками
inline
ListLogElements operator << ( const String &lhv, const String &rhv )
{
	ListLogElements tmp;
	SmartPtr< LexicalElement > lhvTmp( new LexicalElement( lhv ) );
	SmartPtr< LexicalElement > rhvTmp( new LexicalElement( rhv ) );
	tmp.push_back( (const SmartPtr< ILogElement >&) lhvTmp );
	tmp.push_back( (const SmartPtr< ILogElement >&) rhvTmp );
	return tmp;
}

inline
ListLogElements operator << ( const String &lhv, ListLogElements rhv )
{
	SmartPtr< LexicalElement > lhvTmp( new LexicalElement( lhv ) );
	rhv.push_back( (const SmartPtr< ILogElement >&) lhvTmp );
	return rhv;
}

inline
ListLogElements operator << ( ListLogElements lhv, const String &rhv )
{
	SmartPtr< LexicalElement > rhvTmp( new LexicalElement( rhv ) );
	lhv.push_back( (const SmartPtr< ILogElement >&) rhvTmp );
	return lhv;
}

template< class RHV >
inline
ListLogElements operator << ( const String &lhv, const RHV &rhv )
{
	ListLogElements tmp;
	SmartPtr< LexicalElement > lhvTmp( new LexicalElement( lhv ) );
	SmartPtr< RHV > rhvTmp( new RHV() );
	tmp.push_back( (const SmartPtr< ILogElement >&) lhvTmp );
	tmp.push_back( (const SmartPtr< ILogElement >&) rhvTmp );
	return tmp;
}

template< class LHV >
inline
ListLogElements operator << ( const LHV &lhv, const String &rhv )
{
	ListLogElements tmp;
	SmartPtr< LexicalElement > rhvTmp( new LexicalElement( rhv ) );
	SmartPtr< LHV > lhvTmp( new LHV() );
	tmp.push_back( (const SmartPtr< ILogElement >&) lhvTmp );
	tmp.push_back( (const SmartPtr< ILogElement >&) rhvTmp );
	return tmp;
}

// Разбираемся с остальными типами
template< class LHV >
inline
ListLogElements operator << ( const LHV &lhv, ListLogElements rhv )
{
	SmartPtr< LHV > lhvTmp( new LHV() );
	rhv.push_back( (const SmartPtr< ILogElement >&) lhvTmp );
	return rhv;
}

template< class RHV >
inline
ListLogElements operator << ( ListLogElements lhv, const RHV &rhv )
{
	SmartPtr< RHV > rhvTmp( new RHV() );
	lhv.push_back( (const SmartPtr< ILogElement >&) rhvTmp );
	return lhv;
}

} // namespace logging
} // FatRat Library

#endif // frl_logging_elements_h_
