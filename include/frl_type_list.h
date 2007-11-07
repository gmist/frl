#ifndef FRL_TYPE_LIST_H_
#define FRL_TYPE_LIST_H_

#include "frl_null_type.h"

namespace frl
{
	
	template< class H , class T >
	class TypeList
	{
		typedef H head;
		typedef T tail;
	};

	#define TYPE_LIST_1( T1 ) TypeList< T1, NullType >
	#define TYPE_LIST_2( T1, T2 ) TypeList< T1, TYPE_LIST_1( T2 ) >
	#define TYPE_LIST_3( T1, T2, T3 ) TypeList< T1, TYPE_LIST_2( T2, T3 ) >
	#define TYPE_LIST_4( T1, T2, T3, T4 ) TypeList< T1, TYPE_LIST_3( T2, T3, T4 ) >
	#define TYPE_LIST_5( T1, T2, T3, T4, T5 ) TypeList< T1, TYPE_LIST_4( T2, T3, T4, T5 ) >
	#define TYPE_LIST_6( T1, T2, T3, T4, T5, T6 ) TypeList< T1, TYPE_LIST_5( T2, T3, T4, T5, T6 ) >
	#define TYPE_LIST_7( T1, T2, T3, T4, T5, T6, T7 ) TypeList< T1, TYPE_LIST_6( T2, T3, T4, T5, T6, T7 ) >
	#define TYPE_LIST_8( T1, T2, T3, T4, T5, T6, T7, T8 ) TypeList< T1, TYPE_LIST_7( T2, T3, T4, T5, T6, T7, T8 ) >
	#define TYPE_LIST_9( T1, T2, T3, T4, T5, T6, T7, T8, T9 ) TypeList< T1, TYPE_LIST_8( T2, T3, T4, T5, T6, T7, T8, T9 ) >

	namespace type_list
	{
		template < class TList > struct Length;
		template <> struct Length < NullType >
		{
			enum { value = 0 };
		};

		template< class H, class T >
		struct Length< TypeList< H, T > >
		{
			enum { value = 1 + Length<T>::value };
		};

		// Distributed hierarchy generator
		template< class TList, template < class > class Unit >
		class DistrGen;

		template < class T1, class T2, template <class> class Unit >
		class DistrGen< TypeList< T1, T2 >, Unit >
			:	public DistrGen< T1, Unit >,
			public DistrGen< T2, Unit >
		{
		public:
			typedef TypeList< T1, T2 > typeList;
			typedef DistrGen< T1, Unit > left;
			typedef DistrGen< T2, Unit > rigth;

		};

		template< class Type, template< class > class Unit >
		class DistrGen : public Unit< Type >
		{
			typedef Unit< Type > left;
		};

		template< template < class > class Unit >
		class DistrGen< NullType, Unit >
		{
		};		

		template < class T >
		struct DefaultHolder
		{
			T value;
		};

	} // namespace type_list
}

#endif  /* FRL_TYPE_LIST_H_ */
