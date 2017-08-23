#include <iostream>
#include <settings_class.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

void Settings_Profile::test_function(int a){
	std::cout << a;
	return;
}

Settings_Profile::Settings_Profile(){
}

Settings_Profile::Settings_Profile(std::string filename){
	try{
		boost::property_tree::ptree data;
		boost::property_tree::xml_parser::read_xml(filename, data);
		std::string inputs = data.get<std::string>("db.profile.img_name");
		this->addImgName(inputs);
		inputs = data.get<std::string>("db.profile.config_name");
		this->setConfigName(inputs);
		inputs = data.get<std::string>("db.profile.base_dir");
		this->setBaseDir(inputs);
		float coord = data.get<float>("db.profile.x");
		this->addX(coord);
		coord = data.get<float>("db.profile.y");
		this->addY(coord);
		this->default_res_x = data.get<int>("db.profile.default_res_x");
		this->default_res_y = data.get<int>("db.profile.default_res_y");
	}catch(const std::exception &e){
		std::cerr << e.what();
		exit(1);
	}
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
	std::cout << "Config Name: " << this->config_name << "\nBase Dir: " << this->base_dir << "\nImage Names: " << img_names.at(0) << "\nX: " << this->x.at(0) << "\nY: " << this->y.at(0) << "\n";
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