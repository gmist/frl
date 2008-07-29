#ifndef frl_singleton_h_
#define frl_singleton_h_

namespace frl{

template< class T >
class SingletonMeyers
{
private:
	SingletonMeyers();
	SingletonMeyers( const SingletonMeyers& );
	SingletonMeyers& operator = ( const SingletonMeyers& );
	~SingletonMeyers(){};
public:
	static T& getInstance()
	{
		static T tmp;
		return tmp;
	}
}; // class SingletonMeyers

} // FatRat Library

#endif // frl_singleton_h_
