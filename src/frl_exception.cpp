#include "frl_exception.h"
#include "stream_std/frl_sstream.h"

namespace frl
{
	Exception *Exception::last = NULL;
	String Exception::msFunctionStack[ FRL_CALL_STACK_DEPTH ];
	UShort Exception::msStackDepth = 0;

	Exception::Exception( const String &description_ )
		:	line( 0 ),
			description( description_ ),
			function( FRL_STR("") ),
			stackDepth( msStackDepth )
	{
		last = this;
	}

	Exception::Exception( const String &description_, const String &function_ )
		:	line( 0 ),
			description( description_ ),
			function( function_ ),
			stackDepth( msStackDepth )
	{
		last = this;
	}

	Exception::Exception( const frl::String &function_, const String &file_, frl::ULong line_ )
		:	line( line_ ),
			description( FRL_STR("Unknown description") ),
			function( function_ ),
			file( file_ ),
			stackDepth( msStackDepth )
	{
		last = this;
	}

	Exception::Exception( const frl::String &description_, const frl::String &function_, const String &file_, frl::ULong line_ )
		:	line( line_ ),
			description( description_ ),
			function( function_ ),
			file( file_ ),
			stackDepth( msStackDepth )
	{
		last = this;
	}

	Exception::Exception( const Exception &rvl )
		:	std::exception( rvl ), // for make happy MinGW
			line( rvl.line ),
			description( rvl.description ),
			function( rvl.function ),
			file( rvl.file )
	{
	}

	const Exception& Exception::operator = ( const Exception &rvl )
	{
		if( this == &rvl)
			return *this;
		description = rvl.description;
		function = rvl.function;
		file = rvl.file;
		line = rvl.line;
		return *this;
	}

	Exception::~Exception() throw()
	{
	}

	const char*	Exception::what() const throw()
	{
		Exception *This = const_cast<Exception*>( this );
		This->whatDescription = multiByteCompatibility( getFullDescription() ).c_str();
		return whatDescription.c_str();
	}

	String Exception::getFullDescription(void) const
	{
		frl::stream_std::StringStream desc;

		desc	<<	FRL_STR( "\n" )
				<<	FRL_STR( "-----------------------------------\n")
				<<	FRL_STR( "An exception has been thrown!\n" )
				<<	FRL_STR( "Details:\n" )
				<<	FRL_STR( "-----------------------------------\n" )
				<<	FRL_STR( "Description: " ) << description
				<<	FRL_STR( "\nFunction name: " ) << function
				<<	FRL_STR( ". \n" )
				<<	FRL_STR( "-----------------------------------\n" );

		#if( FRL_COMPILER == FRL_COMPILER_MINGW  )
		if( line > (ULong)(0) )
		#else
		if( line > 0 )
		#endif
		{
			desc << FRL_STR( "\nFile: " ) << file.c_str();
			desc << FRL_STR( "\nLine: " ) << line;
		}

		desc << "\nStack unwinding:\n";
		for(  UShort stackUnroll = stackDepth <= FRL_CALL_STACK_DEPTH ? ( stackDepth - 1 ) : ( FRL_CALL_STACK_DEPTH - 1 );
				stackUnroll < stackDepth;
				--stackUnroll )
		{
			desc << msFunctionStack[ stackUnroll ] << FRL_STR(" <= " );
		}

		desc << FRL_STR( "<< beginning of stack >>\n" );
		return desc.str();
	}

	//-----------------------------------------------------------------------
	void Exception::pushFunction( const String& strFuncName ) throw()
	{
		if( msStackDepth < FRL_CALL_STACK_DEPTH )
			msFunctionStack[ msStackDepth ] = strFuncName;
		++msStackDepth;
	}

	//-----------------------------------------------------------------------
	void Exception::popFunction() throw()
	{
		--msStackDepth;
	}
}
