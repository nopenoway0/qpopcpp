#ifndef SCREEN_CAPTURER_H
#define SCREEN_CAPTURER_H

#include <iostream>
#include "ancillary_functions.h"
#include "gdiplus.h"
namespace Screen_Capturer{

	using namespace Ancillary_Function;

	int takeSnapshot(Gdiplus::Bitmap** m, std::string p_name);
	result_data takeSnapshot(std::string p_name, result_data (*foo)(void*, void*), void* arg);
	float saveHBITMAP_SC(void* bmp, void* name);
	float compHBITMAP_WSC(void* sc_bmp, void* o_bmp);
	result_data compHBITMAP_SEL(void* sc_bmp, void* o_bmp);

}

#endif