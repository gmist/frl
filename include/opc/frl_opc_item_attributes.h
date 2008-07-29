#ifndef frl_opc_item_attributes_h_
#define frl_opc_item_attributes_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_item.h"

namespace frl{ namespace opc{

class ItemAttributes
{
private:
	OPCITEMATTRIBUTES* attributes;
	void freeStrings();
public:

	ItemAttributes();
	ItemAttributes( const ItemAttributes& other );
	~ItemAttributes();

	ItemAttributes& operator = ( const ItemAttributes& rhv );
	ItemAttributes& operator = ( const std::pair< OPCHANDLE, GroupItemElem >& newItem );
	void copyTo( OPCITEMATTRIBUTES& dst );
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_item_attributes_h_
