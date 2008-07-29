#ifndef frl_select_h_
#define frl_select_h_

namespace frl{

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

#endif // frl_select_h_
