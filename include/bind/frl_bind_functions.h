#ifndef FRL_BIND_FUNCTIONS_H_
#define FRL_BIND_FUNCTIONS_H_

#include "bind/frl_list.h"

namespace frl
{
	namespace bind
	{
		template < typename R >
		class BindTs0
		{
		private:
			typedef R ( *fn )();
			fn fn_;
		public:
			BindTs0( fn fn_in ) : fn_( fn_in ) { }
			R operator() () { return (*fn_)(); }
		};
		
		template < typename R, typename Arg1, class P1 >
		class BindTs1 : private bind::List1PH< P1 >
		{
		private:
			typedef R ( *fn )( Arg1 );
			fn fn_;

		public:
			BindTs1( fn fn_in  ) : fn_( fn_in )
			{
			}

			R operator () ( Arg1 a1 )
			{
				frl::bind::List1<Arg1 > list( a1 );
				return ( *fn_ )( list[typeArg1] );
			}
		};

		template < class R, class Arg1, class Arg2, class P1, class P2 >
		class BindTs2	 : private bind::List2PH< P1, P2 >
		{
		private:
			typedef R ( *fn )( Arg1, Arg2 );
			fn fn_;

		public:
			BindTs2( fn fn_in ) : fn_( fn_in )
			{
			}

			R operator () ( Arg1 a1, Arg2 a2 )
			{
				frl::bind::List2<Arg1, Arg2> list( a1, a2 );
				return ( *fn_ )( list[typeArg1], list[typeArg2] );
			}
		};
		
		template < class R, class Arg1, class Arg2, class Arg3, class P1, class P2, class P3 >
		class BindTs3 : private bind::List3PH< P1, P2, P3 >
		{
		private:
			typedef R ( *fn )( Arg1, Arg2, Arg3 );
			fn fn_;

		public:
			BindTs3( fn fn_in ) : fn_( fn_in )
			{
			}
			R operator () ( Arg1 a1, Arg2 a2, Arg3 a3 )
			{
				frl::bind::List3<Arg1, Arg2, Arg3> list( a1, a2, a3 );
				return ( *fn_ )( list[typeArg1], list[typeArg2], list[ typeArg3] );
			}
		};

		template
			< 
				class R, class Arg1, class Arg2, class Arg3, class Arg4,
				class P1, class P2, class P3, class P4
			>
		class BindTs4 : private bind::List4PH< P1, P2, P3, P4 >
		{
		private:
			typedef R ( *fn )( Arg1, Arg2, Arg3, Arg4 );
			fn fn_;

		public:
			BindTs4( fn fn_in ) : fn_( fn_in )
			{
			}
			R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4 )
			{
				frl::bind::List4<Arg1, Arg2, Arg3, Arg4> list( a1, a2, a3, a4 );
				return ( *fn_ )( list[ typeArg1 ], list[ typeArg2 ], list[ typeArg3 ], list[ typeArg4 ] );
			}
		};

		template 
			< 
				class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5,
				class P1, class P2, class P3, class P4, class P5
			>
		class BindTs5 : private bind::List5PH< P1, P2, P3, P4, P5 >
		{
		private:
			typedef R ( *fn )( Arg1, Arg2, Arg3, Arg4, Arg5 );
			fn fn_;
			typedef bind::List5PH<P1, P2, P3, P4, P5 > TypeArg;

		public:
			BindTs5( fn fn_in ) : fn_( fn_in )
			{
			}
			R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5 )
			{
				frl::bind::List5<Arg1, Arg2, Arg3, Arg4, Arg5> list( a1, a2, a3, a4, a5 );
				return ( *fn_ )( list[ typeArg1 ], list[ typeArg2 ], list[ typeArg3 ], list[ typeArg4 ], list[ typeArg5 ] );
			}
		};

		template
			<
				class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6,
				class P1, class P2, class P3, class P4, class P5, class P6
			>
		class BindTs6 : private bind::List6PH< P1, P2, P3, P4, P5, P6 >
		{
		private:
			typedef R ( *fn )( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 );
			fn fn_;

		public:
			BindTs6( fn fn_in ) : fn_( fn_in )
			{
			}
			R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6 )
			{
				frl::bind::List6<Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> list( a1, a2, a3, a4, a5, a6 );
				return ( *fn_ )( list[ typeArg1 ], list[ typeArg2 ], list[ typeArg3 ], list[ typeArg4 ], list[ typeArg5 ], list[ typeArg6 ] );
			}
		};

		template
			<
				class R,
				class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7,
				class P1, class P2, class P3, class P4, class P5, class P6, class P7
			>
		class BindTs7 : private bind::List7PH< P1, P2, P3, P4, P5, P6, P7 >
		{
		private:
			typedef R ( *fn )( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 );
			fn fn_;

		public:
			BindTs7( fn fn_in ) : fn_( fn_in )
			{
			}
			R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7 )
			{
				frl::bind::List7<Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 > list( a1, a2, a3, a4, a5, a6, a7 );
				return ( *fn_ )( list[ typeArg1 ], list[ typeArg2 ], list[ typeArg3 ], list[ typeArg4 ], list[ typeArg5 ], list[ typeArg6 ], list[ typeArg7 ] );
			}
		};

		template
			<
				class R,
				class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8,
				class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8
			>
		class BindTs8 : private bind::List8PH< P1, P2, P3, P4, P5, P6, P7, P8 >
		{
		private:
			typedef R ( *fn )( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 );
			fn fn_;

		public:
			BindTs8( fn fn_in ) : fn_( fn_in )
			{
			}
			R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7, Arg8 a8 )
			{
				frl::bind::List8<Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 > list( a1, a2, a3, a4, a5, a6, a7, a8 );
				return ( *fn_ )( list[ typeArg1 ], list[ typeArg2 ], list[ typeArg3 ], list[ typeArg4 ], list[ typeArg5 ], list[ typeArg6 ], list[ typeArg7 ], list[ typeArg8 ] );
			}
		};

		template
			<
				class R,
				class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9,
				class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9
			>
		class BindTs9 : private bind::List9PH< P1, P2, P3, P4, P5, P6, P7, P8, P9 >
		{
		private:
			typedef R ( *fn )( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9 );
			fn fn_;

		public:
			BindTs9( fn fn_in ) : fn_( fn_in )
			{
			}
			R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7, Arg8 a8, Arg9 a9 )
			{
				frl::bind::List9<Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9 > list( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
				return ( *fn_ )( list[ typeArg1 ], list[ typeArg2 ], list[ typeArg3 ], list[ typeArg4 ], list[ typeArg5 ], list[ typeArg6 ], list[ typeArg7 ], list[ typeArg8 ], list[ typeArg9 ] );
			}
		};

	} // namespace bind	

	template< class R >
	bind::BindTs0< R > Bind( R(*fn)() )
	{
		return bind::BindTs0<R>( fn );
	}

	template< class R, class P1 >
	bind::BindTs0< R > Bind( R(*fn)(), const P1& )
	{
		return bind::BindTs0<R>( fn );
	}
	
	template < class R, class Arg1, class P1 >
	bind::BindTs1< R, Arg1, P1 > Bind( R ( *fn ) ( Arg1), const P1& )
	{
		return bind::BindTs1< R, Arg1, P1 > ( fn );
	}

	template < class R, class Arg1, class Arg2, class P1, class P2 >
	bind::BindTs2< R, Arg1, Arg2, P1, P2 > Bind( R ( *fn ) ( Arg1, Arg2), const P1&, const P2&)
	{
		return bind::BindTs2< R, Arg1, Arg2, P1, P2 > ( fn );
	}

	template < class R, class Arg1, class Arg2, class Arg3, class P1, class P2, class P3 >
	bind::BindTs3< R, Arg1, Arg2, Arg3, P1, P2, P3 > Bind( R ( *fn ) ( Arg1, Arg2, Arg3 ), const P1&, const P2&, const P3& )
	{
		return bind::BindTs3< R, Arg1, Arg2, Arg3, P1, P2, P3 > ( fn );
	}

	template 
	< class R, class Arg1, class Arg2, class Arg3, class Arg4,
	class P1, class P2, class P3, class P4 >
	bind::BindTs4< R, Arg1, Arg2, Arg3, Arg4, P1, P2, P3, P4 > Bind( R ( *fn ) ( Arg1, Arg2, Arg3, Arg4 ), const P1&, const P2&, const P3&, const P4& )
	{
		return bind::BindTs4< R, Arg1, Arg2, Arg3, Arg4, P1, P2, P3, P4 > ( fn );
	}

	template 
	<
		class R,
		class Arg1, class Arg2, class Arg3, class Arg4, class Arg5,
		class P1, class P2, class P3, class P4, class P5 >
	bind::BindTs5< R, Arg1, Arg2, Arg3, Arg4, Arg5, P1, P2, P3, P4, P5 > Bind( R ( *fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5 ), const P1&, const P2&, const P3&, const P4&, const P5& )
	{							
		return bind::BindTs5< R, Arg1, Arg2, Arg3, Arg4,Arg5,  P1, P2, P3, P4, P5 > ( fn );
	}
	
	template 
		<
		class R, 
		class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6,
		class P1, class P2, class P3, class P4, class P5, class P6 
		>
	bind::BindTs6< R, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, P1, P2, P3, P4, P5, P6 > 
	Bind( R ( *fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 ), const P1&, const P2&, const P3&, const P4&, const P5&, const P6& )
	{							
		return bind::BindTs6< R, Arg1, Arg2, Arg3, Arg4,Arg5, Arg6,  P1, P2, P3, P4, P5, P6 > ( fn );
	}	

	template
	<
		class R,
		class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7,
		class P1, class P2, class P3, class P4, class P5, class P6, class P7
	>
	bind::BindTs7< R, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, P1, P2, P3, P4, P5, P6, P7 > 
	Bind( R ( *fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 ), const P1&, const P2&, const P3&, const P4&, const P5&, const P6&, const P7& )
	{
		return bind::BindTs7< R, Arg1, Arg2, Arg3, Arg4,Arg5, Arg6, Arg7, P1, P2, P3, P4, P5, P6, P7 > ( fn );
	}

	

	template
	<
		class R,
		class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8,
		class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8
	>
	bind::BindTs8< R, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, P1, P2, P3, P4, P5, P6, P7, P8 > 
	Bind( R ( *fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 ), const P1&, const P2&, const P3&, const P4&, const P5&, const P6&, const P7&, const P8& )
	{
		return bind::BindTs8< R, Arg1, Arg2, Arg3, Arg4,Arg5, Arg6, Arg7, Arg8, P1, P2, P3, P4, P5, P6, P7, P8 > ( fn );
	}

	template
	<
		class R, 
		class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9,
		class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9
	>
	bind::BindTs9< R, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, P1, P2, P3, P4, P5, P6, P7, P8, P9 > 
	Bind( R ( *fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9 ), const P1&, const P2&, const P3&, const P4&, const P5&, const P6&, const P7&, const P8&, const P9& )
	{
		return bind::BindTs9< R, Arg1, Arg2, Arg3, Arg4,Arg5, Arg6, Arg7, Arg8, Arg9, P1, P2, P3, P4, P5, P6, P7, P8, P9 > ( fn );
	}
}

#endif /* FRL_BIND_FUNCTIONS_H_ */
