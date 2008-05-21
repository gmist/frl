#ifndef frl_psoi2_device_h_
#define frl_psoi2_device_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <vector>
#include "frl_io.h"
#include "frl_types.h"
#include "frl_opc.h"
#include "thread/frl_thread.h"
#include "logging/frl_logging.h"
#include <bitset>

struct Psoi2Channel
{
	frl::opc::address_space::Tag *value; // value of chlorine concentration
	frl::opc::address_space::Tag *typePPC; // type of PPC ( 0 - PPC1, 1 - PPC2)
	frl::opc::address_space::Tag *thresholdExceeding; //	0 - concentration of chlorine to be normal,
																				//	1 - concentration chlorine threshold exceeding
	frl::opc::address_space::Tag *goodPPC; // 0 - bad PPC, 1 - is good PPC
	frl::opc::address_space::Tag *goodMGC; // 0 - bag MGC (micro generator of chlorine), 1 - is good MGC
};

class Psoi2Device
{
private:
	std::vector< Psoi2Channel > channels;
	frl::UInt portNumber;
	frl::UInt bytesNumber;
	frl::Bool simulation;
	frl::UInt channelsNumber;
	frl::io::comm_ports::Serial comPort;
	frl::thread::Thread< void, void, Psoi2Device > processThread;
	frl::logging::Logger log;

	void setUpValues( const frl::String &low, const frl::String &hight );
	void setUpThresholdExceeding( const frl::String &low, const frl::String &hight );
	void setUpTypePPC( const frl::String &low, const frl::String &hight );
	void setUpMGC( const frl::String &low, const frl::String &hight );
	void setUpPPC( const frl::String &low, const frl::String &hight );
	void simulationProcess();
	void workProcess();
	void fillValues( const std::vector< std::bitset<8> > &pure_array );
	
	void setValue( frl::UInt number, float value );
	void setTypePPC( frl::UInt number, frl::Bool type );
	void setGoodPPC( frl::UInt number, frl::Bool goodFlag );
	void setGoodMGC( frl::UInt number, frl::Bool goodFlag );
	void setThresholdExceeding( frl::UInt number, frl::Bool thresholdExceedingFlag );

public:
	Psoi2Device(	frl::UInt portNumber_,
						frl::UInt channelsNumber_,
						frl::Bool simulation_,
						frl::logging::Level logLevel,
						const frl::String &logFileNamePrefix );
	void startProcess();
	frl::UInt getChannelsNumber();
	frl::UInt getBytesNumber();
	~Psoi2Device();
}; // class Psoi2Device


#endif // FRL_PLATFORM_WIN32
#endif // frl_psoi2_device_h_
