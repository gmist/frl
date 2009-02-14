#ifndef frl_opc_impl_item_io_h_
#define frl_opc_impl_item_io_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server_base.h"

namespace frl { namespace opc { namespace impl {

/*!
	\brief IOPCItemIO implementation
	\details
	The purpose of IOPCItemIO interface is to provide an extremely easy way for simple applications to obtain OPC data. 
	Programmers should be aware that in most servers, 
	the design of the Group based OPC interfaces will provide much better performance than this interface. 
	In terms of performance, the user of this interface should assume that it will behave as if he were to create a group, 
	add the items, perform a single read or write and then delete the group.
	A second purpose of this interface is to provide a method for writing timestamp and 
	quality information into servers that support this functionality.
*/
class OPCItemIO
	:	public IOPCItemIO,
		virtual public OPCServerBase
{
public:
	virtual ~OPCItemIO();

	STDMETHODIMP Read( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ LPCWSTR *pszItemIDs,
		/* [size_is][in] */ DWORD *pdwMaxAge,
		/* [size_is][size_is][out] */ VARIANT **ppvValues,
		/* [size_is][size_is][out] */ WORD **ppwQualities,
		/* [size_is][size_is][out] */ FILETIME **ppftTimeStamps,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP WriteVQT( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ LPCWSTR *pszItemIDs,
		/* [size_is][in] */ OPCITEMVQT *pItemVQT,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);
}; // class OPCItemIO

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_item_io_h_
