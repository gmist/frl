#ifndef FRL_NON_COPYABLE_H_
#define FRL_NON_COPYABLE_H_

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
} // namespace FatRat Library

#endif /* FRL_NON_COPYABLE_H_ */
