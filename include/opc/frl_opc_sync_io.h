#ifndef frl_opc_sync_io_h_
#define frl_opc_sync_io_h_
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl
{
	namespace opc
	{
		template< class T >
		class SyncIO : public IOPCSyncIO
		{
		public:
			HRESULT STDMETHODCALLTYPE Read( 
				/* [in] */ OPCDATASOURCE dwSource,
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [size_is][size_is][out] */ OPCITEMSTATE **ppItemValues,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				return E_NOTIMPL;
			};

			HRESULT STDMETHODCALLTYPE Write( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [size_is][in] */ VARIANT *pItemValues,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				return E_NOTIMPL;
			};
		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_sync_io_h_ */
