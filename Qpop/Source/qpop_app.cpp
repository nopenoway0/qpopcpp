#define _WINSOCKAPI_
#include <windows.h>

#include "qpop_app.h"
#include "qpop_frame.h"


#include <stdio.h>
#include <string>
#include <settings_class.h>
#include <iostream>
#include <qpop_server.h>
#include <gdiplus.h>
#include <img_processing.h>
#include <screen_capturer.h>
#include <ancillary_functions.h>
#include <shared_access.h>

// TODO: check why connections are dropping randomly - reenable authentication

using namespace Ancillary_Function;
using namespace Qpop::gui;
void MonitorCondition(Qpop_Server* s, int x, int y, std::string exe_name);
void StartQpopBackend(QpopFrame* frame);

bool QpopApp::OnInit(){
	QpopFrame* frame = new QpopFrame("Qpop", wxPoint(50,50), wxSize(400, 400));
	frame->Show(true);
	this->qpop_scanner = new boost::thread(StartQpopBackend, frame);
	return true;
}


/**
* Monitors the condition of whether or not
* @param s        pointer to a running Qpop_server
* @param x        x coordinate
* @param y        y coordinate
* @param exe_name name of process
*/
void MonitorCondition(Qpop_Server* s, int x, int y, std::string exe_name) {
	// Set up port and start server to interact
	// with mobile app
	s->set_port(8000);
	s->start_server();
	while (_close_qpop == 0) {
		boost::mutex::scoped_lock lock(_monitor_pause);
		condition.wait(lock);
		if (s->conditionSatisfied()) {
			SetDelayCount(4);
			ExhaustiveClick(x, y, exe_name);
			s->setCondition(false);
		}
	}
}

void StartQpopBackend(QpopFrame* frame){
	srand(time(NULL));
	wxString auth_num, wxdifference;
	Qpop_Server main_server;
	std::cout << "Starting Main Program" << std::endl;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   	ULONG_PTR gdiplusToken;
   	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
   	bool image_match = false;
	std::string exe_name;
	Gdiplus::Bitmap* snapshot = NULL;
	Gdiplus::Bitmap* bmp_from_file = NULL;
	int result = 0;

	wxStaticText* connect_status_text = (wxStaticText*) (frame->FindWindowById(wxSTATUS_TEXT));
	wxStaticText* diff_text = (wxStaticText*) (frame->FindWindowById(wxDIFF_TEXT));
   	// Create settings profile for league of legends
   	Settings_Profile cur_prof("lol.txt");

   	// TODO: put into subclass
   	connect_status_text->SetForegroundColour(wxColour(wxT("RED")));
   	frame->Refresh();
   	
   	// Create scaling info struct. Keeps default  x and y for when window is resized. The resize it then applied
   	// To the location of the buttons
	loadBitmapFromFile(&bmp_from_file, cur_prof.getImgName(0));
	float difference = 0;
	tagSIZE* img_dimensions = (tagSIZE*) calloc(sizeof(tagSIZE), cur_prof.getImgCount());
	exe_name = cur_prof.getProcessName();

	// Create thread to manage asynchronous requests. The numbers passed in are the coordinates for the
	// ExhaustiveClick call. When conditions for the server are true - it recieves a signal from a phone - 
	// it will call this exhaustive click
    boost::thread monitor_thread(MonitorCondition, &main_server, cur_prof.getX(0) + (int) (bmp_from_file->GetWidth() / 2), cur_prof.getY(0) + (int) (bmp_from_file->GetHeight() / 2), exe_name);
   	auth_num << main_server.getAuthNum();

   	// set text for ip and authnum
   	((wxStaticText*) (frame->FindWindowById(wxAUTH_TEXT)))->SetLabelText(auth_num);

	while(_close_qpop == 0){
		result = Screen_Capturer::takeSnapshot(&snapshot, exe_name);
		if(main_server.is_Connected()) connect_status_text->SetForegroundColour(wxColour(wxT("GREEN")));
		else connect_status_text->SetForegroundColour(wxColour(wxT("RED")));
		frame->Refresh();

		if(result == 0)
		{	


			// Crop snapshot with the given profile coordinate and the width and height of the loaded file
			// This will keep the resolutions the same and attempt to match it with the appropriate picture
			Img_Processing::cropBitmap(&snapshot, cur_prof.getX(0), cur_prof.getY(0), (int) bmp_from_file->GetWidth(), (int) bmp_from_file->GetHeight());
			// mulitiply by 100 since the percentage wil come back <= 1
			difference = Img_Processing::compareMemoryImg(snapshot, bmp_from_file) * 100;

			// Display Difference
			wxdifference.Clear();
			wxdifference << difference;
			diff_text->SetLabelText(wxdifference);

			if(0 < cur_prof.getThresholdAt(0)) image_match = difference > cur_prof.getThresholdAt(0);
			// if the threshold is negative in the settings profile it means compare the difference for less than that threshold amount
			else image_match = difference < (cur_prof.getThresholdAt(0) * -1);

			// send message to mobile according to if the image matched
			if(image_match){

				condition.notify_one();
				if(delay_count <= 0){
					main_server.send("popped");
					// If server is connected, set a 20 loop delay. This prevents a click being sent, the button transition
					// not completing and retriggering the threshold, resulting in a short false positive
					if(main_server.is_Connected()){
						SetDelayCount(20);
					}
				}
			}
			else{
				main_server.send("queued");
				main_server.setCondition(false);
			}
			// deduct from delay count every iteration
			if(delay_count > 0){
				DecDelayCount();
			}
		}
		// Prevent from websocket closing if league isn't running
		else{
			main_server.send("queued");
			main_server.setCondition(false);
		}

		Sleep(500);
	}

	DeleteObject(bmp_from_file);
	DeleteObject(snapshot);
	free(img_dimensions);
	Gdiplus::GdiplusShutdown(gdiplusToken);
}