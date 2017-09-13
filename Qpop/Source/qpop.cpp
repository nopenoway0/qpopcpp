#define _WINSOCKAPI_

#include <stdio.h>
#include <string>
#include <settings_class.h>
#include <iostream>
#include <windows.h>
#include <qpop_server.h>
#include <gdiplus.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <img_processing.h>
#include <screen_capturer.h>
#include <ancillary_functions.h>
#include <shared_access.h>
#define RUNNING_DIR "./"

Qpop_Server main_server;

using namespace Ancillary_Function;

void MonitorCondition(Qpop_Server* server, int x, int y, std::string exe_name);

int main(){
	std::cout << "Starting Main Program" << std::endl;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   	ULONG_PTR gdiplusToken;
   	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
   	bool image_match = false;
	std::string exe_name;
	Gdiplus::Bitmap* snapshot = NULL;
	Gdiplus::Bitmap* bmp_from_file = NULL;
	int result = 0;

   	// Create settings profile for league of legends
   	Settings_Profile cur_prof("lol.txt");

   	// Create scaling info struct. Keeps default  x and y for when window is resized. The resize it then applied
   	// To the location of the buttons
	loadBitmapFromFile(&bmp_from_file, cur_prof.getImgName(0));
	float difference = 0;
	tagSIZE* img_dimensions = (tagSIZE*) calloc(sizeof(tagSIZE), cur_prof.getImgCount());
	exe_name = cur_prof.getProcessName();

	// Start up server to connect with mobile app
	main_server.set_port(8000);
	main_server.start_server();

	// Create thread to manage asynchronous requests. The numbers passed in are the coordinates for the
	// ExhaustiveClick call. When conditions for the server are true - it recieves a signal from a phone - 
	// it will call this exhaustive click
	boost::thread monitor_thread(MonitorCondition, &main_server, cur_prof.getX(0) + (int) (bmp_from_file->GetWidth() / 2), cur_prof.getY(0) + (int) (bmp_from_file->GetHeight() / 2), exe_name);
	while(_close_qpop == 0){
		result = Screen_Capturer::takeSnapshot(&snapshot, exe_name);
		if(result != 0) std::cerr << "Error taking snapshot" << std::endl;
		else{
			// Crop snapshot with the given profile coordinate and the width and height of the loaded file
			// This will keep the resolutions the same and attempt to match it with the appropriate picture
			Img_Processing::cropBitmap(&snapshot, cur_prof.getX(0), cur_prof.getY(0), (int) bmp_from_file->GetWidth(), (int) bmp_from_file->GetHeight());
			// mulitiply by 100 since the percentage wil come back <= 1
			difference = Img_Processing::compareMemoryImg(snapshot, bmp_from_file) * 100;
			if(0 < cur_prof.getThresholdAt(0)) image_match = difference > cur_prof.getThresholdAt(0);
			// if the threshold is negative in the settings profile it means compare the difference for less than that threshold amount
			else image_match = difference < (cur_prof.getThresholdAt(0) * -1);

			// send message to mobile according to if the image matched
			if(image_match){
				condition.notify_one();
				main_server.send("popped");
			}
			else{
				main_server.send("queued");
				main_server.setCondition(false);
				difference = 0;	
			}

			std::cout << "Difference = " << difference << "%" << std::endl;
		}

		Sleep(800);
	}

	DeleteObject(bmp_from_file);
	DeleteObject(snapshot);
	free(img_dimensions);
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}

void MonitorCondition(Qpop_Server *s, int x, int y, std::string exe_name){
	while(_close_qpop == 0){
		boost::mutex::scoped_lock lock(_monitor_pause);
		condition.wait(lock);
		if(main_server.conditionSatisfied()){
			ExhaustiveClick(x, y, exe_name);
			main_server.setCondition(false);
		}
	}
}