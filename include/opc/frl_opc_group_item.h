#ifndef frl_opc_group_item_h_
#define frl_opc_group_item_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"
#include "frl_non_copyable.h"
#include "opc/frl_opc_com_variant.h"

namespace frl
{
	namespace opc
	{
		class GroupItem : private NonCopyable
		{
		private:
			OPCHANDLE clientHandle;
			OPCHANDLE serverHandle;
			Bool actived;
			String accessPath;
			String itemID;
			VARTYPE requestDataType;
			FILETIME lastChange;
			ComVariant cachedValue;
		public:
			GroupItem();
			~GroupItem();
			void Init();
			void Init( OPCHANDLE serverHandle_, OPCITEMDEF &itemDef );
			void isActived( Bool activedFlag );
			Bool isActived();
			void setClientHandle( OPCHANDLE handle );
			void setRequestDataType( VARTYPE type );
			VARTYPE getReguestDataType();
			OPCHANDLE getClientHandle();
			const String& getItemID();
			const String& getAccessPath();
			const ComVariant& readValue();
			HRESULT writeValue( const VARIANT &newValue );
			const FILETIME& getTimeStamp();
			DWORD getAccessRights();
			DWORD getQuality();
			OPCHANDLE getServerHandle();
			Bool isChange();
			static GroupItem* cloneFrom( const GroupItem &rhv );
			const ComVariant& getCachedValue();
		}; // GroupItem
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_group_item_h_ */
