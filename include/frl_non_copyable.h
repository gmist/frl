#ifndef frl_non_copyable_h_
#define frl_non_copyable_h_

namespace frl
{
class NonCopyable
{
protected:
	NonCopyable() {}
	~NonCopyable() {}
private:
	NonCopyable( const NonCopyable& );
	const NonCopyable& operator=( const NonCopyable& );
}; // class NonCopyable
} // FatRat Library

#endif // frl_non_copyable_h_
