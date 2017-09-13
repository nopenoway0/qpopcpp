#include "img_processing.h"
#include "screen_capturer.h"
#include <iostream>
#include <string>
using namespace std;
using namespace Img_Processing;
using namespace Screen_Capturer;

int main(int argc, char* argv[]){
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   	ULONG_PTR gdiplusToken;
   	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
   	std::string program_name;
	Gdiplus::Bitmap* screenshot = NULL;
	float difference = 0;

	if(argc == 2) program_name = argv[1];
	else program_name = "Skype.exe";
	cout << "Program: " << program_name << endl;
	Gdiplus::Bitmap* load_img = new Gdiplus::Bitmap(L"tmp.bmp", NULL);

	for(int x = 0; x< 100000; x++)
	{
		takeSnapshot(&screenshot, program_name);
		cropBitmap(&screenshot, 0, 0, 200, 200);
		difference = compareMemoryImg(load_img, screenshot);
	}

	cout << "Difference is : " << difference * 100 << " %" << endl;
	// Clean up
	DeleteObject(screenshot);
	DeleteObject(load_img);
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}