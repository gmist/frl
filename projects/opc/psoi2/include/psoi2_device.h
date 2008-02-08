#ifndef frl_psoi2_device_h_
#define frl_psoi2_device_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_types.h"
#include "frl_opc.h"

using namespace frl::opc;
struct Psoi2Channel
{
	address_space::Tag *value; // value of chlorine concentration
	address_space::Tag *typePPC; // type of PPC ( 0 - PPC1, 1 - PPC2)
	address_space::Tag *thresholdExceeding; //	0 - concentration of chlorine to be normal,
																  //	1 - concentration chlorine threshold exceeding
	address_space::Tag *goodPPC; // 0 - bad PPC, 1 - is good PPC
	address_space::Tag *goodMGC; // 0 - bag MGC (micro generator of chlorine), 1 - is good MGC
};

class Psoi2Device
{
private:
	DA2Server *server;
	Psoi2Channel channels[20];
	void initializeAddressSpace();
public:
	Psoi2Device();
	~Psoi2Device();
	void setValue( int channelNum, float value );
	void setThresholdExceeding( int channelNum, int isThresholdExceeding );
	void setType( int channelNum, int type );
	void setGoodPPC( int channelNum, int isGood );
	void setGoodMGC( int channelNum, int isGood );
}; // class Psoi2Device


#endif // FRL_PLATFORM_WIN32
#endif // frl_psoi2_device_h_
