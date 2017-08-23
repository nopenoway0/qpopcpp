#ifndef SCREEN_CAPTURER_H
#define SCREEN_CAPTURER_H

#include <iostream>
#include <ancillary_functions.h>

class Screen_Capturer{
	public:
		static struct result_data takeSnapshot(std::string p_name);
		static struct result_data takeSnapshot(std::string p_name, struct result_data (*foo)(void*, void*), void* arg);
		static float saveHBITMAP_SC(void* bmp, void* name);
		static float compHBITMAP_WSC(void* sc_bmp, void* o_bmp);
		static struct result_data compHBITMAP_SEL(void* sc_bmp, void* o_bmp);
};

#endif