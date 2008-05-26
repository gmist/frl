#ifndef frl_bind_list_h_
#define frl_bind_list_h_

#include "bind/frl_place_holders.h"
#include "bind/frl_storage.h"

namespace frl
{
	namespace bind
	{
		class List0
		{
			List0() {}
		};

		template < class A1 >
		class List1 : private Storage1< A1 >
		{
		private:
			typedef Storage1<A1> base_type;
		public:
			explicit List1( A1 a1 ) : base_type( a1 ) {}
			A1 operator[] ( frl::IntToType< 1 > ) { return base_type::arg1; }
		};

		template< class A1 >
		struct List1PH
		{
			A1 typeArg1;
		};

		template< class A1, class A2 >
		class List2 : private Storage2< A1, A2 >
		{
		private:
			typedef Storage2< A1, A2 > base_type;
		public:
			List2( A1 a1, A2 a2 ) : base_type( a1, a2 ) {}
			A1 operator[] ( frl::IntToType< 1 > ) { return base_type::arg1; }
			A2 operator[] ( frl::IntToType< 2 > ) { return base_type::arg2; }
		};

		template< class A1, class A2 >
		struct List2PH : public List1PH< A1 >
		{
			A2 typeArg2;
		};

		template< class A1, class A2, class A3 >
		class List3 : private Storage3< A1, A2, A3 >
		{
		private:
			typedef Storage3< A1, A2, A3 > base_type;
		public:
			List3( A1 a1, A2 a2, A3 a3 ) : base_type( a1, a2, a3 ) {}
			A1 operator[] ( frl::IntToType< 1 > ) { return base_type::arg1; }
			A2 operator[] ( frl::IntToType< 2 > ) { return base_type::arg2; }
			A3 operator[] ( frl::IntToType< 3 > ) { return base_type::arg3; }
		};

		template< class A1, class A2, class A3 >
		struct List3PH : public List2PH< A1, A2 >
		{
			A3 typeArg3;
		};

		template< class A1, class A2, class A3, class A4 >
		class List4 : private Storage4< A1, A2, A3, A4 >
		{
		private:
			typedef Storage4< A1, A2, A3, A4 > base_type;
		public:
			List4( A1 a1, A2 a2, A3 a3, A4 a4 ) : base_type( a1, a2, a3, a4 ) {}
			A1 operator[] ( frl::IntToType< 1 > ) { return base_type::arg1; }
			A2 operator[] ( frl::IntToType< 2 > ) { return base_type::arg2; }
			A3 operator[] ( frl::IntToType< 3 > ) { return base_type::arg3; }
			A4 operator[] ( frl::IntToType< 4 > ) { return base_type::arg4; }
		};

		template< class A1, class A2, class A3, class A4 >
		struct List4PH : public List3PH< A1, A2, A3 >
		{
			A4 typeArg4;
		};

		template< class A1, class A2, class A3, class A4, class A5 >
		class List5 : private Storage5< A1, A2, A3, A4, A5 >
		{
		private:
			typedef Storage5< A1, A2, A3, A4, A5 > base_type;
		public:
			List5( A1 a1, A2 a2, A3 a3, A4 a4, A4 a5 ) : base_type( a1, a2, a3, a4, a5 ) {}
			A1 operator[] ( frl::IntToType< 1 > ) { return base_type::arg1; }
			A2 operator[] ( frl::IntToType< 2 > ) { return base_type::arg2; }
			A3 operator[] ( frl::IntToType< 3 > ) { return base_type::arg3; }
			A4 operator[] ( frl::IntToType< 4 > ) { return base_type::arg4; }
			A5 operator[] ( frl::IntToType< 5 > ) { return base_type::arg5; }
		};

		template< class A1, class A2, class A3, class A4, class A5 >
		struct List5PH : public List4PH< A1, A2, A3, A4 >
		{
			A5 typeArg5;
		};

		template< class A1, class A2, class A3, class A4, class A5, class A6 >
		class List6 : private Storage6< A1, A2, A3, A4, A5, A6 >
		{
		private:
			typedef Storage6< A1, A2, A3, A4, A5, A6 > base_type;
		public:
			List6( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 ) : base_type( a1, a2, a3, a4, a5, a6 ) {}
			A1 operator[] ( frl::IntToType< 1 > ) { return base_type::arg1; }
			A2 operator[] ( frl::IntToType< 2 > ) { return base_type::arg2; }
			A3 operator[] ( frl::IntToType< 3 > ) { return base_type::arg3; }
			A4 operator[] ( frl::IntToType< 4 > ) { return base_type::arg4; }
			A5 operator[] ( frl::IntToType< 5 > ) { return base_type::arg5; }
			A6 operator[] ( frl::IntToType< 6 > ) { return base_type::arg6; }
		};

		template< class A1, class A2, class A3, class A4, class A5, class A6 >
		struct List6PH : public List5PH< A1, A2, A3, A4, A5 >
		{
			A6 typeArg6;
		};

		template< class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
		class List7 : private Storage7< A1, A2, A3, A4, A5, A6, A7 >
		{
		private:
			typedef Storage7< A1, A2, A3, A4, A5, A6, A7 > base_type;
		public:
			List7( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 ) : base_type( a1, a2, a3, a4, a5, a6, a7 ) {}
			A1 operator[] ( frl::IntToType< 1 > ) { return base_type::arg1; }
			A2 operator[] ( frl::IntToType< 2 > ) { return base_type::arg2; }
			A3 operator[] ( frl::IntToType< 3 > ) { return base_type::arg3; }
			A4 operator[] ( frl::IntToType< 4 > ) { return base_type::arg4; }
			A5 operator[] ( frl::IntToType< 5 > ) { return base_type::arg5; }
			A6 operator[] ( frl::IntToType< 6 > ) { return base_type::arg6; }
			A7 operator[] ( frl::IntToType< 7 > ) { return base_type::arg7; }
		};

		template< class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
		struct List7PH : public List6PH< A1, A2, A3, A4, A5, A6 >
		{
			A7 typeArg7;
		};

		template< class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
		class List8 : private Storage8< A1, A2, A3, A4, A5, A6, A7, A8 >
		{
		private:
			typedef Storage8< A1, A2, A3, A4, A5, A6, A7, A8 > base_type;
		public:
			List8( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 ) : base_type( a1, a2, a3, a4, a5, a6, a7, a8 ) {}
			A1 operator[] ( frl::IntToType< 1 > ) { return base_type::arg1; }
			A2 operator[] ( frl::IntToType< 2 > ) { return base_type::arg2; }
			A3 operator[] ( frl::IntToType< 3 > ) { return base_type::arg3; }
			A4 operator[] ( frl::IntToType< 4 > ) { return base_type::arg4; }
			A5 operator[] ( frl::IntToType< 5 > ) { return base_type::arg5; }
			A6 operator[] ( frl::IntToType< 6 > ) { return base_type::arg6; }
			A7 operator[] ( frl::IntToType< 7 > ) { return base_type::arg7; }
			A8 operator[] ( frl::IntToType< 8 > ) { return base_type::arg8; }
		};

		template< class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
		struct List8PH : public List7PH< A1, A2, A3, A4, A5, A6, A7 >
		{
			A8 typeArg8;
		};

		template< class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9 >
		class List9 : private Storage9< A1, A2, A3, A4, A5, A6, A7, A8, A9 >
		{
		private:
			typedef Storage9< A1, A2, A3, A4, A5, A6, A7, A8, A9 > base_type;
		public:
			List9( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 ) : base_type( a1, a2, a3, a4, a5, a6, a7, a8, a9 ) {}
			A1 operator[] ( frl::IntToType< 1 > ) { return base_type::arg1; }
			A2 operator[] ( frl::IntToType< 2 > ) { return base_type::arg2; }
			A3 operator[] ( frl::IntToType< 3 > ) { return base_type::arg3; }
			A4 operator[] ( frl::IntToType< 4 > ) { return base_type::arg4; }
			A5 operator[] ( frl::IntToType< 5 > ) { return base_type::arg5; }
			A6 operator[] ( frl::IntToType< 6 > ) { return base_type::arg6; }
			A7 operator[] ( frl::IntToType< 7 > ) { return base_type::arg7; }
			A8 operator[] ( frl::IntToType< 8 > ) { return base_type::arg8; }
			A9 operator[] ( frl::IntToType< 9 > ) { return base_type::arg9; }
		};

		template< class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9 >
		struct List9PH : List8PH< A1, A2, A3, A4, A5, A6, A7, A8 >
		{
			A9 typeArg9;
		};
	} // namespace bind
} // FatRat Library

#endif // frl_bind_list_h_
