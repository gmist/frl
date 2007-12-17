#ifndef FRL_EXCEPTION_H_
#define FRL_EXCEPTION_H_

#include <vector>
#include "frl_platform.h"
#include "frl_config.h"
#include "frl_types.h"
#include "frl_lock.h"
#include "frl_string.h"
#include "frl_non_copyable.h"
#include "frl_auto_value.h"
#include "sys/frl_sys_util.h"

namespace frl
{
	#if defined ( UNICODE )
		#define FRL_FUNCTION_NAME __FUNCTIONW__
	#else
		#define FRL_FUNCTION_NAME __FUNCTION__
	#endif

	#define FRL_THROW_S() throw( frl::Exception( FRL_FUNCTION_NAME, __FILE__, __LINE__ ) )
	#define FRL_THROW( description ) throw( frl::Exception( description, FRL_FUNCTION_NAME, __FILE__, __LINE__ ) )
	#define FRL_THROW_EX( description, function ) throw( frl::Exception( description, function, __FILE__, __LINE__ ) )
	#define FRL_THROW_S_CLASS( xClass ) throw( xClass( FRL_FUNCTION_NAME, __FILE__, __LINE__ ) )
	#define FRL_THROW_CLASS( xClass, description ) throw( xClass( description, FRL_FUNCTION_NAME, __FILE__, __LINE__ ) )
	#define FRL_THROW_CLASS_EX( xClass, description, function ) throw( xClass( description, function, __FILE__, __LINE__ ) )

#define FRL_THROW_SYSAPI( description ) throw( frl::Exception(description + frl::String( FRL_STR( " " ) ) + frl::sys::util::getLastErrorDescription(), FRL_FUNCTION_NAME, __FILE__, __LINE__ ) );
#define FRL_THROW_SYSAPI_EX( description, error ) throw( frl::Exception(description + frl::String( FRL_STR( " " ) ) + frl::sys::util::getCodeErrorDescription(error), FRL_FUNCTION_NAME, __FILE__, __LINE__ ) );

#define FRL_EXCEPT_GUARD_EX( a ) frl::Exception::ExceptionGuard exceptionGuardObject( ( a ) )
#define FRL_EXCEPT_GUARD() frl::Exception::ExceptionGuard exceptionGuardObject( ( FRL_FUNCTION_NAME ) )

class Exception
			:	public std::exception 
		{
		protected:
			AutoValue< ULong > line;
			String description;
			String function;
			String file;
			std::string whatDescription;	
			AutoValue< UShort > stackDepth;

			// Static members.
			static Exception *last;
			static String msFunctionStack[ FRL_CALL_STACK_DEPTH ];
			static UShort msStackDepth;

			// Default constructor.
			Exception();
		public:

			// Constructor.
			Exception( const String &description_);

			// Constructor.
			Exception( const String &description_, const String &function_ );

			// Constructor.
			Exception( const String &function_, char *file_, ULong line_);

			// Constructor.
			Exception( const String &description_, const String &function_, char *file_, ULong line_ );

			// Copy constructor.
			Exception( const Exception &rvl );

			// Destructor.
			virtual ~Exception( void ) throw();

			// Assignment operator.
			const Exception& operator = ( const Exception &rvl );

			/// Performs an assignment
			//Exception &operator =( const Exception &other );  // TODO

			/// Returns descriptive message
			const char *what() const throw();

			// Return string with full description( error code, description error, function name ... ) of exception.
			String GetFullDescription(void) const;

			// Return string with only description of exception.
			const String& GetDescription( void ) const throw() { return description; }
			
			// Return string with only source file name of exception.
			const String& GetFile( void ) const throw() { return file; }

			// Return string with only function name of exception.
			const String& GetFunction( void ) const throw() { return function; }

			// Retrieves a pointer to the last exception created.		
			static Exception* GetLastException(void) throw() { return last; }

			// Pushed a function on the stack.
			static void PushFunction( const String& strFuncName ) throw();
			
			// Pops a function from the stack.
			static void PopFunction() throw();
			
			// Class for automatically push/pop the function name for unwinding stack.
			class ExceptionGuard
				:	private NonCopyable
			{
			public:
				ExceptionGuard(const String& funcName) throw()
				{
					Exception::PushFunction(funcName);
				}
				~ExceptionGuard() throw()
				{
					Exception::PopFunction();
				}
			};
		};

#define FRL_EXCEPTION_CLASS( xClass )\
class xClass : public frl::Exception\
{\
public:\
	xClass( const String &description_) : Exception( description_ ) {}\
	xClass( const String &function_, char *file_, ULong line_) : Exception( function_, file_, line_ ) {}\
	xClass( const String &description_, const String &function_ ) : Exception( description_, function_ ) {}\
	xClass( const String &description_, const String &function_, char *file_, ULong line_ ) : Exception( description_, function_, file_, line_ ) {}\
};

} // Fat Rat Library

#endif /* FRL_EXCEPTION_H_ */
