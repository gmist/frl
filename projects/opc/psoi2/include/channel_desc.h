#ifndef channel_desc_h_
#define channel_desc_h_
#include "frl_types.h"

struct ChannelDescr
{
	frl::UInt portNumber;
	frl::UInt channelNumber;
	ChannelDescr();
	ChannelDescr( frl::UInt port, frl::UInt channel );
};

#endif // channel_desc_h_
