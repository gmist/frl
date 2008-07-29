#ifndef frl_time_sys_time_h_
#define frl_time_sys_time_h_
#include <time.h>
#include "frl_types.h"
#include "frl_exception.h"

namespace frl{ namespace time{

class SysTime
{
private:
	String timeSeparator;
	String dateSeparator;
	static inline void fillTM( tm &tmStruct );

public:
	FRL_EXCEPTION_CLASS( InvalidParameter );
	FRL_EXCEPTION_CLASS( UnknownError );

	SysTime();
	~SysTime();

	void setDateSeparator( const String &separator );
	void setTimeSeparator( const String &separator );

	static String getYearYYYY();
	static String getYearYY();
	static String getMonth();
	static String getDay();

	static String getHours();
	static String getMinutes();
	static String getSeconds();

	String getDateYYYYMMDD();
	String getDateYYMMDD();
	String getDateDDMMYYYY();
	String getDateDDMMYY();
	String getDateMMDDYYYY();
	String getDateMMDDYY();

	String getTimeHHMMSS();
};

} // namespace time
} // FatRat Library

#endif // frl_time_sys_time_h_
