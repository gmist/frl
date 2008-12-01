#include "channel_desc.h"

ChannelDescr::ChannelDescr() : portNumber( 0 ), channelNumber( 0 )
{
}

ChannelDescr::ChannelDescr( frl::UInt port, frl::UInt channel ) :	portNumber( port ), channelNumber( channel )
{
}
