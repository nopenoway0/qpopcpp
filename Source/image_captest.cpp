#include <screen_capturer.h>
#include <img_processing.h>
#include <windows.h>
#include <winuser.h>
#include <wingdi.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <ostream>
#include <gdiplus.h>
#include <ancillary_functions.h>

#define SLEEP_TIME 500

int main(){
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   	ULONG_PTR gdiplusToken;
   	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	//Screen_Capturer::takeSnapshot("LolClient.exe", Screen_Capturer::saveHBITMAP_SC, (void*) "test.png");
	//HBITMAP t1 = GetHBITMAPFromImageFile("test.png");
	HBITMAP t2 = GetHBITMAPFromImageFile("play.png");
	struct Pic_Slice tmp;
	tmp.bmp = t2;
	tmp.x = 560;
	tmp.y = 14;
	float difference = 0;
	float cur_diff = 0;
	while(1){
		// VALUE MEANS DIFFERENT THINGS ON LINUX VS WINDOWS!
		Sleep(500);
		cur_diff = *((Screen_Capturer::takeSnapshot("LolClient.exe", Screen_Capturer::compHBITMAP_SEL, (void*) &tmp)).difference); // Ugly change to more elegant way
		if(difference != cur_diff){
			difference = cur_diff;
			std::cout << "Difference: " << difference << "\n";
		}
	}
	//*************************
	
	//*************************
	//DeleteObject(t1);
	DeleteObject(t2);
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}