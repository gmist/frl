#ifndef frl_opc_flat_data_cache_h_
#define frl_opc_flat_data_cache_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include <vector>
#include "opc/frl_opc_cache_item.h"
#include "opc/frl_opc_util.h"
#include "frl_lock.h"

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
			frl::lock::Mutex scopeGuard;
		public:
			typedef std::pair< OPCHANDLE, CacheItem > itemsListElement;
			typedef std::pair< String, OPCHANDLE > nameIndexListElement;;
			
			AddTagResult AddTag( const String& name )
			{
				AddTagResult res;
				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
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
				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
				for( it=itemsList.begin(); it!=itemsList.end(); ++it )
				{
					items.push_back( (*it).second.getID() );
				}
				return;
			}

			Bool isExistItem( const String &itemID )
			{
				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
				return nameIndexList.find( itemID ) != nameIndexList.end();
			}

			Bool isExistItem( OPCHANDLE itemID )
			{
				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
				return itemsList.find( itemID ) != itemsList.end();
			}

			Bool getItem( const String &itemID, CacheItem& item )
			{
				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
				std::map< String, OPCHANDLE >::iterator itName = nameIndexList.find( itemID );
				if( itName == nameIndexList.end() )
					return False;
				guard.~ScopeGuard();
				return getItem( (*itName).second, item );
			}

			Bool getItem( OPCHANDLE handle, CacheItem& item )
			{
				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
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
