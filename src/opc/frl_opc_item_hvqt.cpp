#include "opc/frl_opc_item_hvqt.h"

namespace frl{ namespace opc{

ItemHVQT::ItemHVQT()
	:	qualitySpecified( False ),
		timeStampSpecified( False )
{
}

ItemHVQT::ItemHVQT( const ItemHVQT& other )
	:	handle( other.handle ),
		value( other.value ),
		quality( other.quality ),
		timeStamp( other.timeStamp ),
		qualitySpecified( other.qualitySpecified ),
		timeStampSpecified( other.timeStampSpecified )
{
}

ItemHVQT::ItemHVQT(	OPCHANDLE handle_,
									const os::win32::com::Variant& value_,
									WORD quality_,
									const FILETIME& timeStamp_ )
	:	handle( handle_ ),
		value( value_ ),
		quality( quality_ ),
		timeStamp( timeStamp_ ),
		qualitySpecified( True ),
		timeStampSpecified( True )
{
}

ItemHVQT::~ItemHVQT()
{
}

ItemHVQT& ItemHVQT::operator=( const ItemHVQT& rhvl )
{
	if( this == &rhvl )
		return *this;
	handle = rhvl.handle;
	value = rhvl.value;
	quality = rhvl.quality;
	timeStamp = rhvl.timeStamp;
	qualitySpecified = rhvl.qualitySpecified;
	timeStampSpecified = rhvl.timeStampSpecified;
	return *this;
}

OPCHANDLE ItemHVQT::getHandle() const
{
	return handle;
}

const os::win32::com::Variant& ItemHVQT::getValue() const
{
	return value;
}

WORD ItemHVQT::getQuality() const
{
	return quality;
}

const FILETIME& ItemHVQT::getTimeStamp() const
{
	return timeStamp;
}

void ItemHVQT::setHandle( OPCHANDLE newHandle )
{
	handle = newHandle;
}

void ItemHVQT::setValue( const os::win32::com::Variant& newValue )
{
	value = newValue;
}

void ItemHVQT::setQuality( WORD newQuality )
{
	quality = newQuality;
	qualitySpecified = True;
}

void ItemHVQT::setTimeStamp( const FILETIME& newTimeStamp )
{
	timeStamp = newTimeStamp;
	timeStampSpecified = True;
}

frl::Bool ItemHVQT::isQualitySpecified() const
{
	return qualitySpecified;
}

frl::Bool ItemHVQT::isTimeStampSpecified() const
{
	return timeStampSpecified;
}

} // namespace opc
} // FatRat Library
