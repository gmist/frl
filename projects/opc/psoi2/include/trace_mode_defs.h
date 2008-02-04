#ifndef trace_mode_defs_h_
#define trace_mode_defs_h_

namespace trace_mode_defs
{
	typedef union
	{
		unsigned char c[6];
		unsigned short int i[3];
	}IA;

	typedef union
	{
		unsigned char c[4];
		unsigned short int i[2];
		unsigned long  l;
		float f;
	}FOUR_BYTE;

	typedef union
	{
		unsigned char c[2];
		unsigned short int i;
	}TWO_BYTE;

	typedef struct
	{
		union
		{
			float v;
			unsigned char c[4];
			unsigned short int i[2];
			int d;
		}V;

		union
		{
			unsigned char fmt[4];
			unsigned short int ind[2];
		}F;
	}RSDATA;
}

#endif // trace_mode_defs_h_