#include <sys/types.h>
#include <sys/timeb.h>
#include "time/frl_time_sys_time.h"
#include "frl_platform.h"
#include "stream_std/frl_sstream.h"

namespace frl
{
namespace time
{

SysTime::SysTime()
	:	timeSeparator( FRL_STR(":") ),
		dateSeparator( FRL_STR("-") )
{

}

SysTime::~SysTime()
{

}

void SysTime::setDateSeparator( const String &separator )
{
	FRL_EXCEPT_GUARD();
	if( separator.empty() )
		FRL_THROW_S_CLASS( SysTime::InvalidParameter );
	dateSeparator = separator;
}

void SysTime::setTimeSeparator( const String &separator )
{
	FRL_EXCEPT_GUARD();
	if( separator.empty() )
		FRL_THROW_S_CLASS( SysTime::InvalidParameter );
	timeSeparator = separator;
}

frl::String SysTime::getYearYYYY()
{
	FRL_EXCEPT_GUARD();
	tm tmStruct;
	SysTime::fillTM( tmStruct );
	stream_std::StringStream stream;
	stream << ( tmStruct.tm_year + 1900 );
	return stream.str();
}

frl::String SysTime::getMonth()
{
	FRL_EXCEPT_GUARD();
	tm tmStruct;
	SysTime::fillTM( tmStruct );
	stream_std::StringStream stream;
	tmStruct.tm_mon += 1;
	if( tmStruct.tm_mon < 10 )
		stream << 0;
	stream << tmStruct.tm_mon;
	return stream.str();
}

frl::String SysTime::getDay()
{
	FRL_EXCEPT_GUARD();
	tm tmStruct;
	SysTime::fillTM( tmStruct );
	stream_std::StringStream stream;
	if( tmStruct.tm_mday < 10 )
		stream << 0;
	stream << tmStruct.tm_mday;
	return stream.str();
}

void SysTime::fillTM( tm &tmStruct )
{
	FRL_EXCEPT_GUARD();

	#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		__timeb64 tstruct;
		_ftime64_s(&tstruct);
		errno_t  ret = _localtime64_s( &tmStruct, &tstruct.time );
		if( ret != 0 )
			FRL_THROW_S_CLASS( SysTime::UnknownError );
	#else
		_timeb tstruct;
		_ftime(&tstruct );
		tmStruct = *localtime( &tstruct.time );
	#endif // FRL_COMPILER_MSVC
	#else
		__timeb64 tstruct;
		_ftime64(&tstruct);
		tmStruct = *_localtime64(&tstruct.time);
	#endif
}

frl::String SysTime::getDateYYYYMMDD()
{
	return SysTime::getYearYYYY() +
			dateSeparator +
			SysTime::getMonth() +
			dateSeparator +
			SysTime::getDay();
}

frl::String SysTime::getYearYY()
{
	FRL_EXCEPT_GUARD();
	tm tmStruct;
	SysTime::fillTM( tmStruct );
	stream_std::StringStream stream;
	if( tmStruct.tm_year > 100  && tmStruct.tm_year < 200 )
		tmStruct.tm_year -= 100;
	if( tmStruct.tm_year > 200 )
		tmStruct.tm_year -= 200;
	stream << "0";
	stream << tmStruct.tm_year;
	return stream.str();
}

frl::String SysTime::getDateYYMMDD()
{
	FRL_EXCEPT_GUARD();
	return SysTime::getYearYY() +
			dateSeparator +
			SysTime::getMonth() +
			dateSeparator +
			SysTime::getDay();
}

frl::String SysTime::getDateDDMMYYYY()
{
	FRL_EXCEPT_GUARD();
	return SysTime::getDay() +
			dateSeparator +
			SysTime::getMonth() +
				dateSeparator +
			SysTime::getYearYYYY();
}

frl::String SysTime::getDateDDMMYY()
{
	FRL_EXCEPT_GUARD();
	return SysTime::getDay() +
			dateSeparator +
			SysTime::getMonth() +
			dateSeparator +
			SysTime::getYearYY();
}

frl::String SysTime::getDateMMDDYYYY()
{
	FRL_EXCEPT_GUARD();
	return SysTime::getMonth() +
			dateSeparator +
			SysTime::getDay() +
			dateSeparator +
			SysTime::getYearYYYY();
}

frl::String SysTime::getDateMMDDYY()
{
	FRL_EXCEPT_GUARD();
	return SysTime::getMonth() +
			dateSeparator +
			SysTime::getDay() +
			dateSeparator +
			SysTime::getYearYY();
}

frl::String SysTime::getHours()
{
	FRL_EXCEPT_GUARD();
	tm tmStruct;
	SysTime::fillTM( tmStruct );
	stream_std::StringStream stream;
	if( tmStruct.tm_hour < 10 )
		stream << 0;
	stream << tmStruct.tm_hour;
	return stream.str();
}

frl::String SysTime::getMinutes()
{
	FRL_EXCEPT_GUARD();
	tm tmStruct;
	SysTime::fillTM( tmStruct );
	stream_std::StringStream stream;
	if( tmStruct.tm_min < 10 )
		stream << 0;
	stream << tmStruct.tm_min;
	return stream.str();
}

frl::String SysTime::getSeconds()
{
	FRL_EXCEPT_GUARD();
	tm tmStruct;
	SysTime::fillTM( tmStruct );
	stream_std::StringStream stream;
	if( tmStruct.tm_sec < 10 )
		stream << 0;
	stream << tmStruct.tm_sec;
	return stream.str();
}

frl::String SysTime::getTimeHHMMSS()
{
	FRL_EXCEPT_GUARD();
	return SysTime::getHours() +
			timeSeparator +
			SysTime::getMinutes() +
			timeSeparator +
			SysTime::getSeconds();
}
} // namespace time
} // FatRat Library
