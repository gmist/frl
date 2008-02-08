#ifndef frl_opc_async_request_h_
#define frl_opc_async_request_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <list>
#include "..\dependency\vendors\opc_foundation\opcda.h"
#include "frl_types.h"
#include "frl_exception.h"

namespace frl
{
	namespace opc
	{
		class AsyncRequest
		{
		private:
			DWORD id;
			DWORD cancelID;
			Bool cancelled;
			std::list< OPCHANDLE > handles;
			VARIANT *values;
			DWORD source;
		public:
			FRL_EXCEPTION_CLASS( InvalidParameter );
			AsyncRequest();
			AsyncRequest( std::list< OPCHANDLE > handles_ );
			AsyncRequest( const AsyncRequest &rhv );
			~AsyncRequest();
			AsyncRequest& operator = ( const AsyncRequest &rhv );
			void setTransactionID( DWORD id_ );
			DWORD getTransactionID() const;
			DWORD getCancelID();
			Bool isCancelled();
			void isCancelled( Bool isCancelled_ );
			void init( std::list< OPCHANDLE > handles_, const VARIANT *values_ );
			const std::list<OPCHANDLE>& getHandles() const;
			size_t getCounts() const;
			const VARIANT* getValues() const;
			void removeHandles( OPCHANDLE handle );
			DWORD getSource() const;
			void setSource( DWORD source_ );
			static DWORD getUniqueCancelID();
		}; // class AsyncRequest
	} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM == FRL_PLATFORM_WIN32
#endif // frl_opc_async_request_h_
