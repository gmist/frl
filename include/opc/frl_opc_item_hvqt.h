#ifndef frl_opc_item_hvqt_h_
#define frl_opc_item_hvqt_h_
#include "frl_types.h"
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "os/win32/com/frl_os_win32_com_variant.h"

namespace frl
{
namespace opc
{
class ItemHVQT
{
private:
	OPCHANDLE handle;
	os::win32::com::Variant value;
	WORD quality;
	FILETIME timeStamp;

	Bool timeStampSpecified;
	Bool qualitySpecified;
public:
	ItemHVQT();
	ItemHVQT(	OPCHANDLE handle_,
						const os::win32::com::Variant& value_,
						WORD quality_,
						const FILETIME& timeStamp_ );

	ItemHVQT( const ItemHVQT& other );
	~ItemHVQT();
	ItemHVQT& operator = ( const ItemHVQT& rhvl );

	OPCHANDLE getHandle() const;
	const os::win32::com::Variant& getValue() const;
	WORD getQuality() const;
	const FILETIME& getTimeStamp() const;

	void setHandle( OPCHANDLE newHandle );
	void setValue( const os::win32::com::Variant& newValue );
	void setQuality( WORD newQuality );
	void setTimeStamp( const FILETIME& newTimeStamp );
	
	Bool isQualitySpecified() const;
	Bool isTimeStampSpecified() const;
}; // class ItemHVQT

} // namespace opc
} // FatRat Library

#endif // frl_opc_item_hvqt_h_
