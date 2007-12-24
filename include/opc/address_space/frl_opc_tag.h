#ifndef frl_opc_tag_h_
#define frl_opc_tag_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <list>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"
#include "frl_exception.h"
#include "opc/frl_opc_com_variant.h"
#include <atlcomcli.h>

namespace frl
{
	namespace opc
	{
		namespace address_space
		{
			FRL_EXCEPTION_CLASS( IsExistTag );
			FRL_EXCEPTION_CLASS( NotExistTag );
			FRL_EXCEPTION_CLASS( IsNotBranch );
			FRL_EXCEPTION_CLASS ( IsNotLeaf );

			class Tag
			{
			private:
				String id;
				String shortID;
				OPCHANDLE clientHandle, serverHandle;
				Bool is_Branch;
				VARTYPE requestedDataType, canonicalDataType;
				DWORD accessRights;
				Bool active;
				String delimiter;
				Tag *parent;
				std::list< Tag* > tags;
				ComVariant value;
				DWORD quality;
				FILETIME timeStamp;
				DWORD scanRate;

				Tag();

				void addTag( const String &name, Bool is_Branch_ );

			public:

				Tag( Bool is_Branch_, const String &delimiter_ );

				~Tag();

				void setID( const String& newID );

				const String& getID();

				const String& getShortID();

				void setClientHandle( OPCHANDLE newHandle );

				OPCHANDLE getClientHandle();

				void setServerHandle( OPCHANDLE newHandle );

				OPCHANDLE getServerHandle();

				Bool isBranch();

				Bool isLeaf();

				void setRequestedDataType( VARTYPE newType );

				VARTYPE getReguestedDataType();

				void setCanonicalDataType( VARTYPE newType );

				VARTYPE getCanonicalDataType();

				void setAccessRights( DWORD newAccessRights );

				DWORD getAccessRights();

				void isActive( Bool actived );

				Bool isActive();

				void isWriteable( Bool writeable );

				Bool isWriteable();

				void addLeaf( const String &name );

				Bool isExistTag( const String &name );

				void addBranch( const String &name );

				Tag* getBranch( const String &name );

				Tag* getLeaf( const String &name );

				Tag* getParent();

				void setParent( Tag* parent_ );

				void browseBranches( std::vector< String > &branches );

				void browseLeafs( std::vector< String > &leaf, DWORD accessFilter = 0 );

				const ComVariant& read();

				void write( const ComVariant &newVal );

				void setQuality( DWORD quality_ );

				DWORD getQuality();

				ComVariant getTimeStamp();

				void setScanRate( DWORD scanRate_ );

				DWORD getScanRate();
			};
		} // namespace address_space
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_tag_h_*/
