#ifndef FRL_SELECT_H_
#define FRL_SELECT_H_

namespace frl
{
	template < bool flag, class TYPE, class Y >
	struct Select
	{
		typedef TYPE Result;
	};

	template < false, class TYPE, class Y >
	struct Select
	{
		typedef Y Result;
	};
} // FatRat Library

#endif // FRL_SELECT_H_
