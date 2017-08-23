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
	std::cout << "Starting Main Program";
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   	ULONG_PTR gdiplusToken;
   	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	// Load the presets from the folder. If not available, create presets.
	FILE* default_settings; // Lists all contained profiles. Should be altered when a profile is added
	FILE* default_profile; // Contains the original LoL profile for testing
	_profile_name = "League of Legends";
	boost::property_tree::ptree data;
	boost::property_tree::ptree profiles;
	boost::property_tree::ptree tmp_ptree;
	std::deque<float> threshold_holder;
	_close_qpop = 0;
	struct Scaling_Info info;
	if(!(default_settings = fopen("default.xml", "r"))){
		std::cout << "Settings file not found\n";
		// add comments and spacing
		default_settings = fopen("default.xml", "w");
		default_profile = fopen("lol.xml", "w");
		fwrite("<total></total><path><profile_path></profile_path></path>", sizeof(char), 68, default_settings); // Write base XML class for profile paths
		fwrite("<db><profile><config_name></config_name><default_res_x></default_res_x><default_res_y></default_res_y><process></process><base_dir></base_dir><images><x></x><y></y><img_name></img_name><threshold></threshold></images></profile></db>\0", sizeof(char), 232, default_profile); // Write base XML for a "profile"
		fclose(default_settings); // add error check for this seciton
		fclose(default_profile);
		try{
			boost::property_tree::xml_parser::read_xml("lol.xml", data);
			// Add check for lol directory
			if(!boost::filesystem::is_directory("./lol/")){
				//TODO: create assets in lol, such as pictures
				boost::filesystem::create_directory("lol");
			}
			std::cout << "Inputting Default Values\n";
			//Input defaults into lol.xml These values were found in a previous project
			data.put("db.profile.config_name", "League of Legends");
			data.put("db.profile.base_dir", "./lol/");
			data.put("db.profile.images.img_name", "play.png"); //ad img_name for variety of images of join buttons
			data.put("db.profile.process", "LolClient.exe");
			//offset
			data.put("db.profile.default_res_x", 1280);
			data.put("db.profile.default_res_y", 720);
			data.put("db.profile.images.x", 561);
			data.put("db.profile.images.y", 14);
			data.put("db.profile.images.threshold", 60);
			//second image
			tmp_ptree.put("img_name", "aram.png");
			//offset
			tmp_ptree.put("x", 497);
			tmp_ptree.put("y", 435);
			tmp_ptree.put("threshold", 79);
			data.add_child("db.profile.images", tmp_ptree);
			//Set up default value - league of legends in this case
			boost::property_tree::xml_parser::write_xml("lol.xml", data);
			// Move to proper directory
			boost::filesystem::rename("lol.xml", "./lol/lol.xml");
			//install();
		}catch(const std::exception &e){
			std::cerr << e.what();
			exit(1);
		}

		//Close data ptrees?
		try{
			//Insert where to load directory
			boost::property_tree::xml_parser::read_xml("default.xml", profiles);
			profiles.put("path.profile_path", std::string(RUNNING_DIR) + "lol/lol.xml");
			profiles.put("total", 1);
			boost::property_tree::xml_parser::write_xml("default.xml", profiles);
		}catch( const std::exception &e){
			std:: cerr << e.what();
			exit(1);
		}
	}
	else{
		fclose(default_settings);
		boost::property_tree::xml_parser::read_xml("default.xml", profiles);
		std::cout << "Settings File found\n";
	}
	boost::thread thread1(ModifyThread);
	// Load Profiles
	int num_profiles = profiles.get<int>("total");
	Settings_Profile* profile_list = new Settings_Profile[num_profiles];
	boost::property_tree::ptree::const_iterator iter = profiles.begin();
	boost::property_tree::ptree::const_iterator img_iter;
	boost::property_tree::ptree temp_tree;
	iter++;
	std::string file_name;
	std::cout << "number of profiles to load: " << num_profiles << std::endl;
	for(int x = 0; x < num_profiles; x++){ 
		file_name = iter->second.get_child("profile_path").data();
		std::cout << "Loading profile from " << file_name;
		boost::property_tree::xml_parser::read_xml(file_name, data);
		std::cout << "\nLoading: " << data.get<std::string>("db.profile.config_name") << "\n";
		profile_list[x].setConfigName(data.get<std::string>("db.profile.config_name"));
		try{
			img_iter = data.get_child("db.profile").begin();
			//Iterate until images field is reached
			while(std::string(img_iter->first.data()).compare("images") != 0) img_iter++;
			std::string field = img_iter->first.data();
			//img_iter->first.data();
			//std::string field = img_iter->second.get_child("images").data();
			std::string float_holder;
			while(!field.compare("images")){
				profile_list[0].addImgName(img_iter->second.get_child("img_name").data());
				std::cout << "Test Image Name: " << img_iter->second.get_child("img_name").data();
				float_holder = img_iter->second.get_child("x").data();
				profile_list[0].addX(std::stof(float_holder, NULL));
				float_holder = img_iter->second.get_child("y").data();
				profile_list[0].addY(std::stof(float_holder, NULL));
				threshold_holder.push_back(std::stof(img_iter->second.get_child("threshold").data(), NULL));
				img_iter++;
				field = img_iter->first.data();
			}
		}catch(std::exception e){
			std::cerr << "Error occured: " << e.what();
		}
		//profile_list[x].setX(data.get<float>("db.profile.x"));
		//profile_list[x].setY(data.get<float>("db.profile.y"));
		profile_list[x].setBaseDir(data.get<std::string>("db.profile.base_dir"));
		profile_list[x].setProcessName(data.get<std::string>("db.profile.process"));
		profile_list[x].setDefaultRes(data.get<int>("db.profile.default_res_x"), data.get<int>("db.profile.default_res_y"));
		iter++;
		std::cout << "Loading finished\n";
	}
	std::cout << "Length of images: " << profile_list[0].getImgCount();

	//GUI set current profile placeholder - use global identifier
	Settings_Profile cur_prof = profile_list[0];
	for(int x = 0; x < cur_prof.getImgCount(); x++){
		std::cout << "\nLoading Image: " << cur_prof.getBaseDir() << cur_prof.getImgName(x) << "\n";
		if(!(boost::filesystem::exists(cur_prof.getBaseDir() + cur_prof.getImgName(x)))){
			std::cerr << "\n\tImage file " + cur_prof.getBaseDir() << cur_prof.getImgName(x) << " doesn't exist\n";
		exit(1);
		}
	}

	info.pic_data_array = (struct Pic_Slice*) calloc(sizeof(Pic_Slice), cur_prof.getImgCount());
	for(int x = 0; x < cur_prof.getImgCount(); x++){
		info.pic_data_array[x].bmp = GetHBITMAPFromImageFile(cur_prof.getBaseDir() + cur_prof.getImgName(x));
		info.pic_data_array[x].x = cur_prof.getX(x);
		info.pic_data_array[x].y = cur_prof.getY(x);; 
	}
	//**************************Set correct base resolution - it's hardcoded here**************************
	//*****************************************************************************************************
	info.x_scaler = cur_prof.getDResX();
	info.y_scaler = cur_prof.getDresY();
	//*****************************************************************************************************

	std::cout <<"X Scaler: " << info.x_scaler << "Y Scaler: " << info.y_scaler << std::endl;

	float cur_diff = 0;
	float difference = 0;
	tagSIZE* img_dimensions = (tagSIZE*) calloc(sizeof(tagSIZE), cur_prof.getImgCount());
	Gdiplus::Bitmap* tmp = NULL;
	std::string exe_name = cur_prof.getProcessName();
	// Pointer so it can bet deleted before GDUIplusShutdown is called. Otherwise crashing occurs
	for(int x = 0; x < cur_prof.getImgCount(); x++){
		tmp = new Gdiplus::Bitmap(info.pic_data_array[x].bmp, NULL);
		img_dimensions[x].cx = tmp->GetWidth();
		img_dimensions[x].cy = tmp->GetHeight();
	}
	delete tmp;
	info.num_images = cur_prof.getImgCount();
	std::cout << "\nAll Images loaded and dimensions set\n";
	std::cout << "\nWaiting for Match...\n";

	struct result_data rdt;

	main_server.set_port(8000);
	main_server.start_server();
	//_monitor_pause.lock();
	boost::thread monitor_thread(MonitorCondition, &main_server, &info, img_dimensions, exe_name);
	// put click into seperate thread for immediate results
	while(_close_qpop == 0){
		_close_qpop_lock.lock();
		if(_close_qpop == 1){
			_close_qpop_lock.unlock();
			thread1.join();
			// changed to break return 0;
			break;
		}
		_close_qpop_lock.unlock();
		rdt = Screen_Capturer::takeSnapshot(exe_name, Screen_Capturer::compHBITMAP_SEL, (void*) &info);
		std::cout << "comparison finished: \n";
		if(rdt.difference != NULL){
			for(int x = 0; x < cur_prof.getImgCount(); x++){
				printf("Pic %d: diff: %f threshold %f\n", x, rdt.difference[x], threshold_holder.at(x));
				if(threshold_holder.at(x) >0){
					if(rdt.difference[x] < threshold_holder.at(x) && rdt.difference[x] >= 0){
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
				if(threshold_holder.at(x) < 0){
					if(rdt.difference[x] > threshold_holder.at(x)*-1 && rdt.difference[x] >= 0){
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
	thread1.join();
	delete info.pic_data_array;
	delete img_dimensions;
	threshold_holder.clear();
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