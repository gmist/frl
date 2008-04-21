#include "logging/frl_logging.h"

int main( int argc, char *argv[] )
{
	using namespace frl;
	logging::Logger log( FRL_STR("test") );
	log.addDestination( logging::ConsoleWriter() );
	log.addDestination( logging::FileWriter() );
	log.addDestination( logging::DebugWindowWriter() );
	log.setFormat( logging::DateElement()
						<< String( FRL_STR("|") )
						<< logging::TimeElement()
						<< String( FRL_STR("|") )
						<< logging::MsgElement()
						<< logging::EndlElement() );
	FRL_LOG_TRACE( log ) << FRL_STR("test1");
	FRL_LOG_TRACE( log ) << FRL_STR("tест2");
	return 0;
}
