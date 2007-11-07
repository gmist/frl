#ifndef frl_opc_flat_data_cache_h_
#define frl_opc_flat_data_cache_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include <vector>
#include "opc/frl_opc_cache_item.h"
#include "opc/frl_opc_util.h"

namespace frl
{
	namespace opc
	{
		struct AddTagResult
		{
			Bool result;
			OPCHANDLE index;
			operator Bool ()
			{
				return result;
			}
		};

		class FlatDataCache
		{
		private:
			std::map< OPCHANDLE, CacheItem > itemsList;
			std::map< String, OPCHANDLE > nameIndexList;
		public:
			typedef std::pair< OPCHANDLE, CacheItem > itemsListElement;
			typedef std::pair< String, OPCHANDLE > nameIndexListElement;;
			AddTagResult AddTag( const String& name )
			{
				AddTagResult res;
				if( nameIndexList.find( name ) != nameIndexList.end() )
				{
					res.index = -1;
					res.result = False;
				}
				else
				{
					res.index = util::getUniqueServerHandle();
					res.result = True;
					CacheItem item;
					item.setID( name );
					item.setServerHandle(res.index);
					itemsList.insert( itemsListElement( res.index, item ) );
					nameIndexList.insert( nameIndexListElement( name, res.index ) );
				}
				return res;
			}

			void Browse( std::vector< String > &items )
			{
				if( !items.empty() )
					items.erase( items.begin(), items.end() );

				std::map< OPCHANDLE, CacheItem >::iterator it;
				for( it=itemsList.begin(); it!=itemsList.end(); ++it )
				{
					items.push_back( (*it).second.getID() );
				}
			}

			Bool isExistItem( const String &itemID )
			{
				return nameIndexList.find( itemID ) != nameIndexList.end();
			}

			Bool isExistItem( OPCHANDLE itemID )
			{
				return itemsList.find( itemID ) != itemsList.end();
			}

			Bool getItem( const String &itemID, CacheItem& item )
			{
				std::map< String, OPCHANDLE >::iterator itName = nameIndexList.find( itemID );
				if( itName == nameIndexList.end() )
					return False;
				return getItem( (*itName).second, item );
			}

			Bool getItem( OPCHANDLE handle, CacheItem& item )
			{
				std::map< OPCHANDLE, CacheItem >::iterator itHandle = itemsList.find( handle );
				if( itHandle == itemsList.end() )
					return False;
				item = (*itHandle).second;
				return True;
			}
		}; // class FlatDataCache
		extern FlatDataCache flatDataCache;
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_flat_data_cache_h_ */
