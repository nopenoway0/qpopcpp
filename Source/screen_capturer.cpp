#include <windows.h>
#include <screen_capturer.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <winuser.h>
#include <wingdi.h>
#include <img_processing.h>
//#include <ancillary_functions.h>

using namespace Ancillary_Function;

namespace Screen_Capturer{

	int takeSnapshot(Gdiplus::Bitmap** m, std::string p_name){
		HWND main_window = retrieveHWND(p_name);
		if(main_window == 0) return -1;
		RECT dimensions;
		delete *m; // clear old image

		GetWindowRect(main_window, &dimensions);
		//***********************************************************************************************************************
		//http://stackoverflow.com/questions/7292757/how-to-get-screenshot-of-a-window-as-bitmap-object-in-c : Davide Piras******
		//***********************************************************************************************************************
	    HDC hdcScreen = GetDC(NULL);
	    //Sets image size
	    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, dimensions.right - dimensions.left, dimensions.bottom - dimensions.top);
	   	HDC pic = CreateCompatibleDC(hdcScreen);
	    SelectObject(pic, hbmp);
		if(!PrintWindow(main_window, pic, PW_CLIENTONLY)){
			*m = NULL;
			//main_window = 0; //if this fails, the process must be closed
			DeleteDC(pic);
			DeleteDC(hdcScreen);
			DeleteObject(hbmp);
			CloseHandle(main_window);
			return -2;
		}
		//***********************************************************************************************************************
		//http://stackoverflow.com/questions/7292757/how-to-get-screenshot-of-a-window-as-bitmap-object-in-c : Davide Piras******
		//***********************************************************************************************************************
	    *m = Gdiplus::Bitmap::FromHBITMAP(hbmp, NULL);
	    DeleteDC(pic);
	    DeleteDC(hdcScreen);
	    DeleteObject(hbmp);
		if(GetHandleInformation(main_window, NULL)) CloseHandle(main_window);
		return 0;
	}

	result_data takeSnapshot(std::string p_name, struct result_data (*foo)(void*, void*), void* arg){	   
		HWND main_window = retrieveHWND(p_name);
		struct result_data rdt;
		rdt.difference = NULL;
		rdt.threshold = NULL;
		std::cout << "Handle in screen capturer " << main_window << "\n";
		if(main_window == 0){
			std::cout << "Main Window still not found\n";
			return rdt;
		}
		RECT dimensions;
		GetWindowRect(main_window, &dimensions);
		//***********************************************************************************************************************
		//http://stackoverflow.com/questions/7292757/how-to-get-screenshot-of-a-window-as-bitmap-object-in-c : Davide Piras******
		//***********************************************************************************************************************
	    HDC hdcScreen = GetDC(NULL);
	    //Sets image size
	    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, dimensions.right - dimensions.left, dimensions.bottom - dimensions.top);
	   	HDC pic = CreateCompatibleDC(hdcScreen);
	    SelectObject(pic, hbmp);
		if(!PrintWindow(main_window, pic, PW_CLIENTONLY)){
			std::cerr << "\nCouldn't Find Process\n";
			main_window = 0; //if this fails, the process must be closed
			DeleteDC(pic);
			DeleteDC(hdcScreen);
			DeleteObject(hbmp);
			CloseHandle(main_window);
			return rdt;
		}
		rdt = (*foo)(hbmp, arg);
		//***********************************************************************************************************************
		//http://stackoverflow.com/questions/7292757/how-to-get-screenshot-of-a-window-as-bitmap-object-in-c : Davide Piras******
		//***********************************************************************************************************************
	    DeleteDC(pic);
	    DeleteDC(hdcScreen);
	    DeleteObject(hbmp);
		CloseHandle(main_window);
		return rdt;
	}

	float saveHBITMAP_SC(void* bmp, void* name){
		HBITMAP hbmp = (HBITMAP) bmp;
		const char* rname = (char*) name;
		return saveHBITMAP(hbmp, rname);
	}

	float compHBITMAP_WSC(void* sc_bmp, void* o_bmp){
		HBITMAP sc_bmp_hb = (HBITMAP) sc_bmp;
		HBITMAP o_bmp_hb = (HBITMAP) o_bmp;
		float result = Img_Processing::compareMemoryImg(sc_bmp_hb, o_bmp_hb);
		return result;
	}

	result_data compHBITMAP_SEL(void* sc_bmp, void* data){
		HBITMAP sc_bmp_hb = (HBITMAP) sc_bmp;
		struct Scaling_Info* scaling = (struct Scaling_Info*) data;
		Gdiplus::Bitmap img1(scaling->pic_data_array->bmp, NULL);
		//std::cout << "Dimensions precrop: " << img1.GetWidth() << "  " << img1.GetHeight() << std::endl;
		return Img_Processing::compareMemoryImgSel(sc_bmp_hb, scaling->pic_data_array->bmp, scaling->pic_data_array->x, scaling->pic_data_array->y, scaling);
	}
}