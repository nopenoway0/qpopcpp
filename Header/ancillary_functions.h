#ifndef ANCILLARY_FUNCTIONS_H
#define ANCILLARY_FUNCTIONS_H

#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <winuser.h>
#include <wingdi.h>
#include <gdiplus.h>
#include <string>

namespace Ancillary_Function{

	BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
	HWND retrieveHWND(std::string p_name);

	struct coordinates{
		int x;
		int y;
	};

	struct handle_data{
		HWND window;
		DWORD pid;
	};

	struct Pic_Slice{
		HBITMAP bmp;
		float x;
		float y;
	};

	struct result_data{
		float* difference;
		float* threshold;
	};

	typedef struct result_data result_data;

	struct Scaling_Info{
		struct Pic_Slice* pic_data_array;
		float x_scaler;
		float y_scaler;
		float scaling_ratio;
		int num_images;
	};
	/**
	 * Moves mouse cursor to given location relative to process
	 * @param  x       x coordinate
	 * @param  y       y coordinate
	 * @param  process name of process
	 * @return]         returns 0
	 */
	inline int MoveHere(int x, int y, std::string process){
		HWND hwn = retrieveHWND(process); 
		if(hwn != 0){
			SendMessage(hwn, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
		}
		CloseHandle(hwn);
		return 0;
	}
	/**
	 * Performs a click at the given coordinates.
	 * @param  x       x coordinate
	 * @param  y       y coordinate
	 * @param  process name of process to perform click on
	 * @return         currently only returns 0
	 */
	inline int ClickHere(int x, int y, std::string process){
		HWND hwn = retrieveHWND(process); 
		if(hwn != 0){
			try{
				SendMessage(hwn, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
				SendMessage(hwn, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(x, y));
			}catch(std::exception e){
				std::cerr << "error" << e.what() << std::endl;
			}
		}
		CloseHandle(hwn);
		return 0;
	}
	/**
	 * Performs a click at the given coordinates.
	 * @param  x       x coordinate
	 * @param  y       y coordinate
	 * @param  hwn     handle to process to perform click on
	 * @return         currently only returns 0
	 */
	inline int ClickHere(int x, int y, HWND hwn){
		if(hwn != 0){
			try{
				SendMessage(hwn, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
				SendMessage(hwn, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(x, y));
			}catch(std::exception e){
				std::cerr << "error" << e.what() << std::endl;
			}
		}
		//CloseHandle(hwn);
		return 0;
	}
	//**************************************************************************************************************
	//******http://stackoverflow.com/questions/31649659/png-jpg-file-to-hbitmap-with-gdi: Chappie Z*****************
	//**************************************************************************************************************
	inline HBITMAP GetHBITMAPFromImageFile(std::string file_name)
	{
		int size = file_name.length();
		std::wstring name = std::wstring(file_name.begin(), file_name.end());
	    HBITMAP result = NULL;
	    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(name.c_str(), false);
	    if (bitmap)
	    {
	        bitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &result);
	        delete bitmap;
	    }
	    return result;
	}
	//**************************************************************************************************************

	// Unused
	inline int verifyImageIntegrity(std::string img1, void* img2){
		return 0;
	}

	/**
	 * Pass in HBMITMAP changes it to a Bitmap keep in memory
	 * @param  hbmp HBITMAP to store
	 * @return      [currently not needed]
	 */
	inline int saveHBITMAPtoMem(HBITMAP hbmp){
	   	const CLSID codec = { 0x557cf406, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } };
		Gdiplus::Bitmap img(hbmp, NULL);
		//int status = img.Save(name, &codec, NULL);
		return 0;
	}
	/**
	 * Saves HBITMAP to a file
	 * @param  hbmp      HBITMAP to save
	 * @param  file_name name of saved file
	 * @return           0 on complete 1 if error occurs
	 */
	inline int saveHBITMAP(HBITMAP hbmp, std::string file_name){
	   	const CLSID codec = { 0x557cf406, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } };
		Gdiplus::Bitmap img(hbmp, NULL); 
		int size = file_name.length();
		wchar_t* name = new wchar_t[size + 1];
		swprintf(name, size + 1, L"%s", file_name.c_str());
		int status = img.Save(name, &codec, NULL);
		if(status){
			std::cerr << "Error saving image: " << status << "\n";
			return 1;
		}
		return 0;
	}

	/**
	 * Saves Bitmap to file
	 * @param  bmp       Bitmap to save
	 * @param  file_name file to save to
	 * @return           1 on failure 0 on completion
	 */
	inline int saveBitMap(Gdiplus::Bitmap* bmp, std::string file_name){
	   	const CLSID codec = { 0x557cf406, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } };
		int size = file_name.length();
		wchar_t* name = new wchar_t[size + 1];
		swprintf(name, size + 1, L"%s", file_name.c_str());
		int status = bmp->Save(std::wstring(file_name.begin(), file_name.end()).c_str(), &codec, NULL);
		if(status){
			std::cerr << "Error saving image: " << status << "\n";
			return 1;
		}
		return 0;
	}

	//Add expected size
	/**
	 * Used to retrieve a window handle to a process  using the Windows API
	 * @param  p_name name of process to get handle to
	 * @return        0 on failure, nonzero on success
	 */
	inline HWND retrieveHWND(std::string p_name){

		//***********************************Seperate into utility funciton************************************
		//*************************************Find PID with process name**************************************
		DWORD* list_processes = (DWORD*) malloc(sizeof(DWORD) * 2048);
		DWORD bytes_returned;
		if(!EnumProcesses(list_processes, sizeof(DWORD) * 2048, &bytes_returned)){
			return 0;
		}
		//Print all processes
		HANDLE process;
		HMODULE* modules = (HMODULE*) calloc(1024, sizeof(HMODULE));
		DWORD bytes_required;
		DWORD pid = -1;
		char* name = (char*) calloc(20, sizeof(char));

		for(int x = 1; x < bytes_returned / sizeof(DWORD); x++){
			process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, list_processes[x]);
			if( NULL == process){
			}
			if(!GetModuleBaseNameA(process, NULL, name, 20)){
			}
			if(!(p_name.compare(std::string(name)))){
				pid = list_processes[x];
				break;
			}
		CloseHandle(process);
		}
		free(name);
		free(modules);
		free(list_processes);

		//*************************************************************************************************************
		if(pid == (DWORD) -1){
			return 0;
		} 

		//add check for failure
		struct handle_data* data = (struct handle_data*) calloc(1, sizeof(struct handle_data));

		data->pid = pid;
		if(EnumWindows(WNDENUMPROC (*EnumWindowsProc), (LPARAM) data)){
			free(data);
			return 0;
		}
		HWND result = data->window;
		free(data);
		return result;
	}

	inline BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam){
		DWORD pid = 0;
		GetWindowThreadProcessId(hwnd, &pid);
		struct handle_data* data = (struct handle_data*) lParam;
		if(data->pid == pid){
			data->window = hwnd;
			//********************************************************
			//Make sure window is bigger than minimized version
			RECT dimensions;
			GetWindowRect(hwnd, &dimensions);
			if(dimensions.right - dimensions.left > 400 && dimensions.bottom - dimensions.top > 400){
				return FALSE;
			}
			return TRUE;
			//********************************************************
			
		}
		return TRUE;
	}
	//Useless, remove
	inline int MakeActive(){
		return 1;
	}
	/**
	 * Print all buttons associated with a process. Doesn't work properly, only used for diagnostic reasons
	 * @param exe_name name of process to print
	 */
	inline void printButtons(std::string exe_name){
		HWND parent = retrieveHWND(exe_name);
		HWND child = FindWindowEx(parent, NULL, NULL, NULL);
		LPSTR name = (LPSTR) calloc(sizeof(char), 250);
		int counter = 0;
		while(child != NULL){
			for(int x = 0; x < counter; x++) child = FindWindowEx(parent, child, NULL, NULL);
			GetClassName(child, (LPWSTR) name, 250);
			std::cout << "Name is: " << name << "\n";
			SendMessage(child, WM_LBUTTONDOWN, 0, 0);
			SendMessage(child, WM_LBUTTONUP, 0, 0);
			counter++;
		}
		CloseHandle(parent);
		CloseHandle(child);
	}
	/**
	inline void extract(std::string file_name){
		// TODO: Extract contents from assets archive
	}*/

	/**
	 * Performs a click on all child processes of a given handle
	 * @param  hwnd   parent process handle
	 * @param  lparam coordinate struct containing x and y coordinates TODO can reuse a window struct for this
	 * @return        returns true until there are no more windows. This callback will not be called again in this case.
	 */
	inline BOOL CALLBACK ClickChildProc(HWND hwnd, LPARAM lparam){
		struct coordinates* input = (struct coordinates*) lparam;
		WINDOWINFO pwi;
		pwi.cbSize = sizeof(WINDOWINFO);
		std::cout << "Using Handle " << hwnd <<std::endl;
		int result = GetWindowInfo(hwnd, &pwi);
		if(result == 0){
			std::cerr << "Error occured in callback: " <<  GetLastError() << std::endl;
			return false;
		}
		else{
			std::cout << "Coordinates: " << pwi.rcWindow.right - pwi.rcWindow.left << "x" << pwi.rcWindow.bottom - pwi.rcWindow.top << std::endl; 
			std::cout << "Perform click at: " << input->x << "x" << input->y << std::endl;
			ClickHere(input->x, input->y, hwnd);
		}
		return true;
	}

	/**
	 * Uses for windows that use multiple process. In the case of the League Launcher, mutiple processes are associated
	 * with the league window. This function cycles through each othese processes and their handles performing
	 * the click operation at the sent coordiantes.
	 * @param x       x coordinate
	 * @param y       y coordinate
	 * @param process name of main process - LoL is LeagueClientUx.exe
	 */
	inline void ExhaustiveClick(int x, int y, std::string process){
		struct coordinates input;
		input.x = x;
		input.y = y;
		HWND proc_handle = retrieveHWND(process);
		if(proc_handle != 0) EnumChildWindows(proc_handle, &ClickChildProc, (LPARAM) &input);
		else std::cerr << "Improper handle in ButtonClick" << std::endl;
		CloseHandle(proc_handle);
	}

	inline std::wstring StringToWString(std::string s){
		return std::wstring(s.begin(), s.end());
	}

	inline void loadBitmapFromFile(Gdiplus::Bitmap** img, std::string filename){
		delete *img;
		*img = new Gdiplus::Bitmap(StringToWString(filename).c_str(), NULL);
	}
}
#endif