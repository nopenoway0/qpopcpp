#include "settings_class.h"

Settings_Profile::Settings_Profile(){
}
/**
 * Takes a filename as a string. Opens file, and streams in the following format:
 * Set config name
 * Process Title
 * base directory - assumed ./ or current directory
 * button's image name
 * Default X resolution
 * Default Y resolution
 * Offset x of button - button's distance from top left corner of window
 * Offset y of button - button's distance from top left corner of window
 * threshold for image

 * 
 * 
 */
Settings_Profile::Settings_Profile(std::string filename = "lol.txt"){
	char* line;
	
	try{
		//parse file into settings profile
		std::ifstream input_file(filename);
		int resx, resy;
		float threshold;
		// Do parsing here
		line = (char*) calloc(256, sizeof(char));

		// Start populating object. 
		// Implement easier way but using function pointer array and while loop
		input_file.getline(line, 256);
		this->setConfigName(line);
		memset(line, 0, 256);

		input_file.getline(line, 256);
		this->setProcessName(line);
		memset(line, 0, 256);

		input_file.getline(line,256);
		this->setBaseDir(line);	

		input_file.getline(line, 256);
		this->addImgName(line);
		memset(line, 0, 256);

		input_file >> resx;
		input_file >> resy;
		this->setDefaultRes(resx, resy);

		input_file >> resx;
		input_file >> resy;
		this->addX(resx);
		this->addY(resy);	

		input_file >> threshold;
		this->addThreshold(threshold);

		input_file.close();		
	}catch(const std::exception &e){
		std::cerr << e.what();
		free(line); // necessary?
	}
	free(line);
}
/*
void Settings_Profile::printableContents(std::string* file_contents){
	file_contents->clear();
	return;

}
*/

int Settings_Profile::writeToFile(){
	std::string content = this->img_names.at(0);
	std::cout << content;
	return 1;
}

void Settings_Profile::setConfigName(std::string name){
	this->config_name = name;
	return;
}
void Settings_Profile::setBaseDir(std::string name){
	this->base_dir = name;
	return;
}
void Settings_Profile::addImgName(std::string name){
	this->img_names.push_back(name);
	return;
}
void Settings_Profile::addX(float x){
	this->x.push_back(x);
	return;
}
void Settings_Profile::addY(float y){
	this->y.push_back(y);
	return;
}

void Settings_Profile::printInfo(){
	std::cout << "Config Name: " << this->config_name << "\nExecutable: " << this->process_name << "\nBase Dir: ";
	std::cout << this->base_dir << "\nImage Names: " << img_names.at(0);
	std::cout << "\nX: " << this->x.at(0) << "\nY: " << this->y.at(0) << "\n";
}
std::string Settings_Profile::getImgName(int a){
	return this->img_names.at(a);
}
std::string Settings_Profile::getConfigName(){
	return this->config_name;
}
std::string Settings_Profile::getBaseDir(){
	return this->base_dir;
}
float Settings_Profile::getX(int a){
	return this->x.at(a);
}
float Settings_Profile::getY(int a){
	return this->y.at(a);
}
std::string Settings_Profile::getProcessName(){
	return this->process_name;
}
void Settings_Profile::setProcessName(std::string name){
	this->process_name = name;
}
int Settings_Profile::getImgCount(){
	return this->img_names.size();
}

void Settings_Profile::setDefaultRes(int x, int y){
	this->default_res_x = x;
	this->default_res_y = y;
}

int Settings_Profile::getDresY(){
	return this->default_res_y;
}

int Settings_Profile::getDResX(){
	return this->default_res_x;
}

void Settings_Profile::addThreshold(float threshold){
	this->threshold.push_back(threshold);
}

float Settings_Profile::getThresholdAt(int a){
	return this->threshold.at(a);
}