#ifndef SETTINGS_PROFILE_H
#define SETTINGS_PROFILE_H

#include <string>
#include <vector>
class Settings_Profile{
	public:
		/**
		 * Used to test class functionality
		 * prints input - must be int
		 * @param a var to print
		 */
		void test_function(int a);
		/**
		 * Instantiates an empty settings profile. All vars are set to empty strings or -1
		 */
		Settings_Profile();
		/**
		 * Instantiates a class using a predefined sttings profile
		 * TODO: Implement error handling
		 * @param settings pointer to xml file to load info from
		 */
		Settings_Profile(std::string filename);
		// doesn't follow OOP rules
		//void printableContents(std::string* contents);
		int writeToFile();
		std::string getConfigName();
		std::string getBaseDir();
		std::string getImgName(int a);
		int getImgCount();
		std::string getProcessName();
		float getX(int a);
		float getY(int a);
		int getDResX();
		int getDresY();
		void setConfigName(std::string name);
		void setBaseDir(std::string name);
		void addImgName(std::string name);
		void addX(float x);
		void addY(float y);
		void printInfo();
		void setDefaultRes(int x, int y);
		void setProcessName(std::string);
	private:
		// Store profile data in xml
		std::vector<float> x; //X offset to find button
		std::vector<float> y; // Y offset to find button
		std::vector<std::string> img_names; //name of string - TODO: make a list to have multiple images
		std::string base_dir; // base directory for profile
		std::string config_name; // config name for profile - e.g. lol.xml
		std::string process_name;
		int default_res_x;
		int default_res_y;
};

#endif