#ifndef frl_opc_group_manager_h_
#define frl_opc_group_manager_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_event.h"
#include "frl_types.h"
#include "frl_smart_ptr.h"
#include "frl_exception.h"

namespace frl
{
namespace opc
{
class Group;
typedef ComPtr< Group > GroupElem;

class GroupManager : private boost::noncopyable
{
public:
	typedef std::map< OPCHANDLE, GroupElem > GroupElemHandlesMap;
	typedef std::map< String, GroupElem > GroupElemNamesMap;

private:
	boost::mutex guard;
	GroupElemHandlesMap handles_map;
	GroupElemNamesMap names_map;
	void insert( GroupElem& group );
	Event stopUpdate;
	boost::thread updateThread;

	void updateGroups();
public:
	FRL_EXCEPTION_CLASS( IsExistGroup );
	FRL_EXCEPTION_CLASS( NotExistGroup );
	GroupManager();
	~GroupManager();
	GroupElem addGroup( String &name );
	bool removeGroup( String &name );
	bool removeGroup( OPCHANDLE handle );
	GroupElem cloneGroup( String &name, String &to_name );
	void renameGroup( const String &name, const String &to_name );
	GroupElem getGroup( OPCHANDLE handle );
	GroupElem getGroup( String &name );
	std::vector< String > getNamesEnum();
	std::vector< GroupElem > getGroupEnum();
	size_t getGroupCount();
}; // class GroupManager

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_group_manager_h_
