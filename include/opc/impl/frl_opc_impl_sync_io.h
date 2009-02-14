#ifndef frl_opc_impl_sync_io_h_
#define frl_opc_impl_sync_io_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_base.h"

namespace frl { namespace opc { namespace impl {

/*!
	\brief
		IOPCSyncIO and IOPCSyncIO2 implementation.
	\details
		IOPCSyncIO allows a client to perform synchronous 
		read and write operations to a server. 
		The operations will run to completion. 
		Refer to the Data Acquisition and Active State Behavior 
		table for an overview of the server data acquisition 
		behavior and it's affect on functionality within this interface.

		This IOPCSyncIO2 interface was added to enhance the existing IOPCSyncIO interface. 
		IOPCSyncIO2 inherits from IOPCSyncIO and therefore all IOPCSyncIO methods 
		defined in IOPCSyncIO are also part of this interface and will not be documented here. 
		Please refer to the IOPCSyncIO interface methods for further details. 
		It is expected that Data Access 3.0 only servers, 
		will implement this interface as opposed to IOPCSyncIO. 
		The purpose of this interface is to provide a group level method for writing 
		timestamp and quality information into servers that support this functionality. 
		In addition, the ability to Read from a group based on a “MaxAge” is provided. 
		This interface differs from the IOPCItemIO interface in that it is group based as opposed to server based.
*/
class SyncIO
	:	public IOPCSyncIO2,
		virtual public GroupBase
{
public:
	virtual ~SyncIO();

	STDMETHODIMP Read( 
		/* [in] */ OPCDATASOURCE dwSource,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][size_is][out] */ OPCITEMSTATE **ppItemValues,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);;

	STDMETHODIMP Write( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ VARIANT *pItemValues,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP ReadMaxAge( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ DWORD *pdwMaxAge,
		/* [size_is][size_is][out] */ VARIANT **ppvValues,
		/* [size_is][size_is][out] */ WORD **ppwQualities,
		/* [size_is][size_is][out] */ FILETIME **ppftTimeStamps,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP WriteVQT( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ OPCITEMVQT *pItemVQT,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);
};

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_sync_io_h_
