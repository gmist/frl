#ifndef frl_opc_enum_group_h_
#define frl_opc_enum_group_h_

#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <objidl.h>
#include <vector>
#include "opc/frl_opc_group.h"

namespace frl
{
	namespace opc
	{
		class EnumGroup : public  IEnumUnknown
		{
		private:
			volatile LONG refCount;
			size_t currentIndex;	//Current element
			std::vector< opc::Group* > groupList;

		public:
			EnumGroup();
			EnumGroup( const std::vector< Group* > &groups );
			~EnumGroup();

			// IUnknown implementation
			STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
			STDMETHODIMP_(ULONG) AddRef( void);
			STDMETHODIMP_(ULONG) Release( void);
			
			// the OPCEnum Functions
			STDMETHODIMP Next (
				ULONG Requested,
				IUnknown **ppGrp,
				ULONG *pActual
				);

			STDMETHODIMP Skip (
				ULONG celt
				);

			STDMETHODIMP Reset(
				void
				);

			STDMETHODIMP Clone( 
				IEnumUnknown **ppEnum
				);
		}; // class EnumGroup
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_enum_group_h_*/
