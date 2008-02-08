#include <Windows.h>
#include "psoi2_device.h"
#include "trace_mode_defs.h"
#include <iostream>

int main( int argc, char *argv[] )
{
	using namespace trace_mode_defs;

	typedef int (*Open_yyy) ( int&, char*, int&, int&, int& );
	typedef int (*Get_yyy) ( IA &, int&, int&, RSDATA*, char*, int& );
	typedef int (*Close_yyy) ( void );

	HINSTANCE hLibrary; 
	Open_yyy open_yyy; 
	Get_yyy get_yyy;
	Close_yyy close_yyy;

	// Load and check driver
	hLibrary = LoadLibrary( FRL_STR("t12sN.dll") );
	if (hLibrary == NULL) 
	{
		::MessageBox( NULL, FRL_STR("Cant load t12sN.dll (Trace Mode 5 driver for PSOI 2)."), FRL_STR("Error"), MB_OK | MB_ICONSTOP );
		return 0;
	}

	open_yyy = (Open_yyy) GetProcAddress(hLibrary, "?Open_yyy@@YAHAAHPAD000@Z" );
	if( open_yyy == NULL )
	{
		::MessageBox( NULL, FRL_STR("Function \"Open_yyy\" not found in driver."), FRL_STR("Error"), MB_OK | MB_ICONSTOP );
		FreeLibrary(hLibrary);
		return 0;
	}

	get_yyy = (Get_yyy) GetProcAddress(hLibrary, "?Get_yyy@@YAHAATIA@@AAH1PAURSDATA@@PAD1@Z" );
	if( get_yyy == NULL )
	{
		::MessageBox( NULL, FRL_STR("Function \"Get_yyy\" not found in driver."), FRL_STR("Error"), MB_OK | MB_ICONSTOP );
		FreeLibrary(hLibrary);
		return 0;
	}

	close_yyy = (Close_yyy) GetProcAddress(hLibrary, "?Close_yyy@@YAHXZ" );
	if( close_yyy == NULL )
	{
		::MessageBox( NULL, FRL_STR("Function \"Close_yyy\" not found in driver."), FRL_STR("Error"), MB_OK | MB_ICONSTOP );
		FreeLibrary(hLibrary);
		return 0;
	}

	// necessary, but not use variables
	int tmpVal;
	char rbuf[2];

	char name[256];	// Trace Mode 5 driver name
	RSDATA data[2];	// call back data
	IA ia;						// ia.i[0] - cannel number, ia.i[1] - channel attribute
	
	// Open driver
	open_yyy( tmpVal, name, tmpVal, tmpVal, tmpVal );
	if( strcmp( name, "psoi2") != 0 )
	{
		::MessageBox( NULL, FRL_STR("t12sN.dll obvious Trace Mode 5 driver, but this not driver for PSOI2"), FRL_STR("Error"), MB_OK | MB_ICONSTOP );
		FreeLibrary(hLibrary);
		return 0;
	}

	// OPC server initialize
	Psoi2Device psoi2;

	Sleep( 2000 ); // Wait for connections

	while( true /* factory.isServerInUse() */ )
	{
		for( int i = 0; i < 20; i++ )
		{
			data[0].V.v = 1;
			ia.i[0] = i; // set channel number
			for( int j = 0; j < 5; j++ )
			{
				ia.i[1] = j; // set channel attribute
				get_yyy( ia,  tmpVal, tmpVal, data, rbuf, tmpVal ); // get data from driver
				switch( j )
				{
					case 0: // value
						psoi2.setValue( i, data[0].V.v );
						std::cout << data[0].V.v << std::endl; // TODO remove me!
					break;

					case 1: // threshold exceeding
						psoi2.setThresholdExceeding( i, ( int )data[0].V.v );
					break;

					case 2: // is good PPC?
						psoi2.setGoodPPC( i, ( int )data[0].V.v );
					break;

					case 3: // is good MGC?
						psoi2.setGoodMGC( i, ( int )data[0].V.v );
					break;
					
					case 4: // PPC type
						psoi2.setType( i, ( int )data[0].V.v );
					break;
				} // switch
			}	
		}
		::Sleep( 10000 );
	}

	// close driver
	close_yyy();

	FreeLibrary( hLibrary );
	return 0;
}