#ifndef test_smart_ptr_x_com_h_
#define test_smart_ptr_x_com_h_

namespace frl{ namespace private_test{
	static int counter = 0;
	static int comRefCounter = 0;
}}



class X_COM
{
	volatile long refCount;
public:
	X_COM() : refCount( 0 )
	{
		++frl::private_test::counter;
	}

	~X_COM()
	{
		--frl::private_test::counter;
	}

	void AddRef()
	{
		++frl::private_test::comRefCounter;
		++refCount;
	}

	long Release()
	{
		--frl::private_test::comRefCounter;
		--refCount;
		if( refCount == 0 )
			delete this;
		return refCount;
	}

};

#endif // test_smart_ptr_x_com_h_
