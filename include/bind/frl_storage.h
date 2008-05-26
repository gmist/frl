#ifndef frl_bind_storage_h
#define frl_bind_storage_h

namespace frl
{
	namespace bind
	{
			template< class A1 >
			struct Storage1
			{
				A1 arg1;
				explicit Storage1( A1 a1 ) : arg1( a1 ) {}
			};

			template< class A1, class A2 >
			struct Storage2 : public Storage1< A1 >
			{
				A2 arg2;
				Storage2( A1 a1, A2 a2 ) : Storage1<A1>( a1 ), arg2( a2 ) {}
			};

			template< class A1, class A2, class A3 >
			struct Storage3 : public Storage2< A1, A2 >
			{
				A3 arg3;
				Storage3( A1 a1, A2 a2, A3 a3 ) : Storage2< A1, A2 >( a1, a2 ), arg3( a3 ){}
			};

			template< class A1, class A2, class A3, class A4 >
			struct Storage4 : public Storage3 < A1, A2, A3 >
			{
				A4 arg4;
				Storage4( A1 a1, A2 a2, A3 a3, A4 a4 )
					: Storage3< A1, A2, A3 >( a1, a2, a3 ), arg4( a4 ) {}
			};

			template< class A1, class A2, class A3, class A4, class A5 >
			struct Storage5 : public Storage4 < A1, A2, A3, A4 >
			{
				A5 arg5;
				Storage5( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
					:	Storage4< A1, A2, A3, A4 >( a1, a2, a3, a4 ), arg5( a5 ){}
			};

			template< class A1, class A2, class A3, class A4, class A5, class A6 >
			struct Storage6 : public Storage5 < A1, A2, A3, A4, A5 >
			{
				A6 arg6;
				Storage6( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
					:	Storage5<A1, A2, A3, A4, A5 >( a1, a2, a3, a4, a5 ), arg6( a6 ){}
			};

			template< class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
			struct Storage7 : public Storage6< A1, A2, A3, A4, A5, A6 >
			{
				A7 arg7;
				Storage7( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 )
					:	Storage6<A1, A2, A3, A4, A5, A7 >( a1, a2, a3, a4, a5, a6 ), arg7( a7 ){}
			};

			template< class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
			struct Storage8 : public Storage7< A1, A2, A3, A4, A5, A6, A7 >
			{
				A8 arg8;
				Storage8( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 )
					:	Storage7<A1, A2, A3, A4, A5, A6, A7 >( a1, a2, a3, a4, a5, a6, a7 ), arg8( a8 ){}
			};

			template< class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9 >
			struct Storage9 : public Storage8< A1, A2, A3, A4, A5, A6, A7, A8 >
			{
				A9 arg9;
				Storage9( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 )
					:	Storage8<A1, A2, A3, A4, A5, A6, A7, A8 >( a1, a2, a3, a4, a5, a6, a7, a8 ), arg9( a9 ){}
			};
	} // namespace bind
} // FatRat Library

#endif // frl_bind_storage_h
