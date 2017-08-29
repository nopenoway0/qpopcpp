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

void ModifyThread();
void MonitorCondition(Qpop_Server* server, struct Scaling_Info* input, tagSIZE* img_dimensions, std::string exe_name);

int main(){
	std::cout << "Starting Main Program" << std::endl;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   	ULONG_PTR gdiplusToken;
   	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	Scaling_Info info;
	struct result_data rdt;

   	// Create settings profile for league of legends
   	Settings_Profile cur_prof("lol.txt");

   	// Create scaling info struct. Keeps default  x and y for when window is resized. The resize it then applied
   	// To the location of the buttons
   	
	info.x_scaler = (float) cur_prof.getDResX();
	info.y_scaler = (float) cur_prof.getDresY();


	

	float cur_diff = 0;
	float difference = 0;
	tagSIZE* img_dimensions = (tagSIZE*) calloc(sizeof(tagSIZE), cur_prof.getImgCount());
	Gdiplus::Bitmap* tmp = NULL;
	std::string exe_name = cur_prof.getProcessName();
	// Pointer so it can be deleted before GDUIplusShutdown is called. Otherwise crashing occurs
	for(int x = 0; x < cur_prof.getImgCount(); x++){
		tmp = new Gdiplus::Bitmap(info.pic_data_array[x].bmp, NULL);
		img_dimensions[x].cx = tmp->GetWidth();
		img_dimensions[x].cy = tmp->GetHeight();
	}
	delete tmp;
	info.num_images = cur_prof.getImgCount();
	std::cout << "\nAll Images loaded and dimensions set\n";
	std::cout << "\nWaiting for Match...\n";

	main_server.set_port(8000);
	main_server.start_server();
	boost::thread monitor_thread(MonitorCondition, &main_server, &info, img_dimensions, exe_name);
	while(_close_qpop == 0){
		rdt = Screen_Capturer::takeSnapshot(exe_name, Screen_Capturer::compHBITMAP_SEL, (void*) &info);
		std::cout << "comparison finished: \n";
		if(rdt.difference != NULL){
			for(int x = 0; x < cur_prof.getImgCount(); x++){
				printf("Pic %d: diff: %f threshold %f\n", x, rdt.difference[x], cur_prof.getThresholdAt(x));
				if(cur_prof.getThresholdAt(x) >0){
					if(rdt.difference[x] < cur_prof.getThresholdAt(x) && rdt.difference[x] >= 0){
						std::cout << "Threshold limit reached, activating at " << (info.pic_data_array[x].x + img_dimensions[x].cx) * info.scaling_ratio - (img_dimensions[x].cx * info.scaling_ratio / 2) << "x" << (info.pic_data_array[x].y + img_dimensions[x].cy) * info.scaling_ratio - (img_dimensions[x].cy * info.scaling_ratio / 2) << std::endl;
						_img_number_lock.lock();
						_img_number = x;
						_img_number_lock.unlock();
						//_monitor_pause.unlock();
						condition.notify_one();
						main_server.send("popped");
					}
					else{
						main_server.send("queued");
						main_server.setCondition(false);
					}
				}
				if(cur_prof.getThresholdAt(x) < 0){
					if(rdt.difference[x] > cur_prof.getThresholdAt(x)*-1 && rdt.difference[x] >= 0){
						std::cout << "Threshold limit reached, activating at " << (info.pic_data_array[x].x + img_dimensions[x].cx) * info.scaling_ratio - (img_dimensions[x].cx * info.scaling_ratio / 2) << "x" << (info.pic_data_array[x].y + img_dimensions[x].cy) * info.scaling_ratio - (img_dimensions[x].cy * info.scaling_ratio / 2) << std::endl;
						_img_number_lock.lock();
						_img_number = x;
						_img_number_lock.unlock();
						//_monitor_pause.unlock();
						condition.notify_one();
						main_server.send("popped");
					}
					else{
						main_server.send("queued");
						main_server.setCondition(false);
					}
				}			}
			delete rdt.difference;
		}
		Sleep(800);
		while(_paused) Sleep(1000);

	}
	delete info.pic_data_array;
	delete img_dimensions;
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}

void MonitorCondition(Qpop_Server *s, struct Scaling_Info* info, tagSIZE* img_dimensions, std::string exe_name){
	int x = 0;
	while(_close_qpop == 0){
		boost::mutex::scoped_lock lock(_monitor_pause);
		_img_number_lock.lock();
		x = _img_number;
		_img_number_lock.unlock();
		condition.wait(lock);
		if(main_server.conditionSatisfied()){
			ExhaustiveClick((info->pic_data_array[x].x + img_dimensions[x].cx) * info->scaling_ratio - (img_dimensions[x].cx * info->scaling_ratio / 2), (info->pic_data_array[x].y + img_dimensions[x].cy) * info->scaling_ratio - (img_dimensions[x].cy * info->scaling_ratio / 2), exe_name);
			main_server.setCondition(false);
		}
	}
}

/**
 * Run in seperate thread to handle changes from the gui. A pipe is created that is read from and put into the decider
 * the decier tells the thread to pause, run, or quit which it then modifies, for safety with a mutex, so that these 
 * rules are applied to the main thread as well.
 */
void ModifyThread(){
	char decider = '0';
	char buffer_read = '3';
	DWORD bytes_read = 0;
	int result = 0;

  HANDLE pipe;
  pipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\deciderpipe"), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_NOWAIT, 1, 8, 8, NMPWAIT_USE_DEFAULT_WAIT, NULL);
  if(pipe == INVALID_HANDLE_VALUE){
  	std::cerr << "\nInvalid Handle Created " << GetLastError() << "\n";
  }
  while(pipe != INVALID_HANDLE_VALUE){
    break;
  }

	if(pipe == INVALID_HANDLE_VALUE){
		std::cerr << "ERROR";
	}
	else std::cout << "SUCESS";

	while(decider != '1'){
		Sleep(200);
		try{
			result = ReadFile(pipe, &buffer_read, 1, &bytes_read, NULL);
		}
		catch(...){
			std::cerr << "Error Occured result: " << result << "Error Code: " << GetLastError() << "\n";
		}
		if(GetLastError() == 109 || result == 0){
			CloseHandle(pipe);
			 pipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\deciderpipe"), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_NOWAIT, 1, 8, 8, NMPWAIT_USE_DEFAULT_WAIT, NULL);
		}
		if(bytes_read > 0) decider = buffer_read;
		if(decider == '1'){
			_close_qpop_lock.lock();
			_close_qpop = decider;
			_close_qpop_lock.unlock();
		}
		if(decider == '2'){
			_paused = 1;
		}
		else _paused = 0;
	}
	CloseHandle(pipe);
	return;
}