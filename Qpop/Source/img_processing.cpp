#include <img_processing.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
//#include <ancillary_functions.h>

//#include <boost/filesystem.hpp>

#define DEFAULT_RESCALE_Y 100
#define DEFAULT_RESCALE_X 100
// change image to same dimensions
namespace Img_Processing{

	/**
	 * Compares and two image files for color differences. 0 being the same
	 * exact image while 1 is completely tonally different images
	 * @param  img1 name of first image
	 * @param  img2 name of second image
	 * @return      the percentage difference in the color of the images
	 */
	float compareImages(std::string img1, std::string img2){
		int sum_diff, temp = 0;
		float per_diff = 0;
		FILE* img1_loaded;
		FILE* img2_loaded;

		if(!(img1_loaded = fopen(img1.c_str(), "r"))) return -1;
		if(!(img2_loaded = fopen(img2.c_str(), "r"))) return -2; // add exception?

		Gdiplus::Bitmap* img1_bmp = new Gdiplus::Bitmap(std::wstring(img1.begin(), img1.end()).c_str(), NULL);
		Gdiplus::Bitmap* img2_bmp = new Gdiplus::Bitmap(std::wstring(img2.begin(), img2.end()).c_str(), NULL);
		per_diff = compareMemoryImg(img1_bmp, img2_bmp);

		// clean up
		fclose(img1_loaded);
		fclose(img2_loaded);
		delete img1_bmp;
		delete img2_bmp;
		DeleteObject(img1_bmp);
		DeleteObject(img2_bmp);
		return per_diff;
	}

	/**
	 * Calculates the overall percentage difference by converting HBITMAP into bitmaps, then
	 * calling the Bitmap version of the function to return the appropriate value. Percentages are from 0 - 1
	 * with a perfect match being 0 and 100% different images being 1
	 * @param  mem_img1 img1 to compare
	 * @param  mem_img2 img2 to compare
	 * @return          the overall percentage difference in the colors in the images
	 */
	float compareMemoryImg(HBITMAP mem_img1, HBITMAP mem_img2){
		float per_difference = 0;

		// Convert HBITMAP to bitmap pointers to pass into optimized compareMemoryImg
		Gdiplus::Bitmap* img1 = new Gdiplus::Bitmap(mem_img1, NULL);
		Gdiplus::Bitmap* img2 = new Gdiplus::Bitmap(mem_img2, NULL);
		per_difference = compareMemoryImg(img1, img2);

		delete img1;
		delete img2;
		DeleteObject(img1);
		DeleteObject(img2);

		return per_difference;
	}

	// deprecated function - hoping to reimplemnt and remove
	struct result_data compareMemoryImgSel(HBITMAP canvas, HBITMAP img, int x, int y, struct Scaling_Info* si_s){
		float per_diff = 0;
		int temp = 0;
		int rgb_diff[3] = {0};
		Gdiplus::Bitmap img1(canvas, NULL);
		vector<Gdiplus::Bitmap*> img2;
		struct result_data rdt;
		rdt.difference = NULL;
		//************************check canvas is bigger than img*********************************
		//****************************************************************************************
		float scale_factor_x = img1.GetWidth() / si_s->x_scaler;
		float scale_factor_y = img1.GetHeight() / si_s->y_scaler ;
		si_s->scaling_ratio = scale_factor_x;
		cout << "Scale factor " << scale_factor_y << scale_factor_x << " " << "\n";
		cout << "Snapshot dimensions after scale factor applied: " << img1.GetHeight() << "x" << img1.GetWidth() << "\n";
		vector<Gdiplus::Bitmap*> newmap;
		int count = si_s->num_images;
		cout << "number of images: " << count << std::endl;
		rdt.difference = (float*) calloc(sizeof(float), count);
		rdt.threshold = (float*) calloc(sizeof(float), count);

		try{
			for(int z = 0; z < count; z++){
				per_diff = 0;
				for(int x = 0; x < 3; x++) rgb_diff[x] = 0;
				img2.push_back(new Gdiplus::Bitmap(si_s->pic_data_array[z].bmp, NULL));
				x = si_s->pic_data_array[z].x;
				y = si_s->pic_data_array[z].y;
				printf("statistics: x: %d y: %d width: %d height: %d scale_factor: %f", x, y, img2.back()->GetHeight(), img2.back()->GetWidth(), scale_factor_x);
				newmap.push_back(img1.Clone(x * scale_factor_x, y * scale_factor_y, img2.back()->GetWidth() * scale_factor_x, img2.back()->GetHeight() * scale_factor_y, PixelFormat32bppARGB));
				img2.back()->SetResolution(DEFAULT_RESCALE_X * scale_factor_x, DEFAULT_RESCALE_Y * scale_factor_y);
				newmap.back()->SetResolution(DEFAULT_RESCALE_X * scale_factor_x, DEFAULT_RESCALE_Y * scale_factor_y);
				Gdiplus::Color c1;
				Gdiplus::Color c2;
				for(int c = 0; c < DEFAULT_RESCALE_X * scale_factor_x; c++){
					for(int b = 0; b < DEFAULT_RESCALE_Y * scale_factor_y; b++){
						newmap.back()->GetPixel(c, b, &c1);
						img2.back()->GetPixel(c, b, &c2);
						temp = c1.GetR() - c2.GetR();
						if(temp < 0) temp *= -1;
						rgb_diff[0] += temp;
						temp = c1.GetB() - c2.GetB();
						if(temp < 0) temp *= -1;
						rgb_diff[1] += temp;
						temp = c1.GetG() - c2.GetG();
						if(temp < 0) temp *= -1;
						rgb_diff[2] += temp;
						//printf("Img 1: R: %03d G: %03d B: %03d\tImg 2: R: %03d G: %03d B: %03d\n", c1.GetR(), c1.GetG(), c1.GetB(), c2.GetR(), c2.GetG(), c2.GetB());
					}
				}
				for(int z = 0; z < 3; z++){
					per_diff += rgb_diff[z];
				}
				per_diff = per_diff / ((DEFAULT_RESCALE_Y * scale_factor_y) * (DEFAULT_RESCALE_X * scale_factor_x));
				rdt.difference[z] = per_diff;
			}
		}catch(std::exception &e){
				std::cerr << "Error: " << e.what();
				exit(1);
		}
		for(int z = 0; z < img2.size() ; z++)delete img2.at(z);
		for(int z = 0; z < newmap.size(); z++) delete newmap.at(z);
		img2.clear();
		newmap.clear();
		return rdt;
	}

	void getBuckets(FILE* img, int* store, int bytes_to_read){
		unsigned char* buffer = (unsigned char*) calloc(bytes_to_read, sizeof(char));
		fread(buffer, sizeof(char), bytes_to_read, img);
		for(int x = 0; x < bytes_to_read; x++){
			store[buffer[x]]++;
		}
		free(buffer);
	}

	/**
	 * Compares two bitmaps and returns the overall color difference as a decimal where 100% difference would be 1.00 and a match would 0 
	 * @param  img1 First image to compare
	 * @param  img2 Second image to compare
	 * @return the overall difference of the colors in the images as a decimal which is <= 1
	 */
	float compareMemoryImg(Gdiplus::Bitmap* img1, Gdiplus::Bitmap* img2){
		// Create bitmap data for later use to lock bits
		Gdiplus::BitmapData* bitmapData1 = new Gdiplus::BitmapData;
		Gdiplus::BitmapData* bitmapData2 = new Gdiplus::BitmapData;
		// initialize and instantiate variables
		float difference, per_diff = 0;
		int height, width, current_pixel_place = 0; // current_pixel_place used to calculations on bitmap because of its stride
		height = img1->GetHeight();
		width = img1->GetWidth();
		if(height > img2->GetHeight()) height = img2->GetHeight();
		if(width > img2->GetWidth()) width = img2->GetWidth();

		// create a rectangle of the length of the largest picture and lock it's bits
		// the lock is necessary for performance
		Gdiplus::Rect rect(0,0, width, height);
		img1->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData1);
		img2->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData2);

		// Set pointers to first part of bitmap line - must be unsigned char to read each byte correctly
		unsigned char* pixels_1 = (unsigned char*) bitmapData1->Scan0;
		unsigned char* pixels_2 = (unsigned char*) bitmapData2->Scan0;

		// Cycle through all bitmap information - compare
		// increment through each pixel, but multiply by 4 when calculating current pixel place
		// due to 4 bytes being read each time, Red Green Blue and Alpha values
		for(int x = 0; x < height; x++){
			for(int y = 0; y < width; y++){
				current_pixel_place = x * bitmapData1->Stride / 4 + y*4;

				//Compare first 3 values for each pixel, red green blue - not in necessarily in this order
				for(int z = 0; z < 2; z ++)
				{
					difference = pixels_1[current_pixel_place + z] - pixels_2[current_pixel_place + z];
					if(difference < 0) difference *= -1;
					per_diff += difference;
				}
			}
		}
		// clean up
		// double delete prevents memory leaks? need to look into why
		img1->UnlockBits(bitmapData1);
		img2->UnlockBits(bitmapData2);
		delete bitmapData1;
		delete bitmapData2;
		DeleteObject(bitmapData1);
		DeleteObject(bitmapData2);

		// return percentage - added up all difference of colors / total pixels * (R + G + B possible values = 765)
		return (per_diff / (width * height * 765));
	}

	/**
	 * Crops a bitmap image to the given coordinates. The Bitmap to be cropped must a pointer
	 * 	as the function works by deleting what is currently at the pointer an redirecting it.
	 * 	THE ORIGINAL BITMAP IS MODIFIED. Make copies in the calling funciton if necessary
	 * @param img    address of pointer to a bitmap that needs to be croped
	 * @param x      top left x coordinate
	 * @param y      top left y coordinate
	 * @param width  width of the desired cropped image
	 * @param height height of the desired cropped image
	 */
	void cropBitmap(Gdiplus::Bitmap** img, int x, int y, int width, int height){
		Gdiplus::Bitmap* temp = (*img)->Clone(x, y, width, height, PixelFormat32bppARGB);
		// delete bitmap before changing location
		delete *img;
		*img = temp;
	}

	/**
	 * Crops a bitmap image to the given coordinates. The Bitmap to be cropped must a pointer
	 * 	as the function works by deleting what is currently at the pointer an redirecting it.
	 * 	The original cropped_img will be modified. Make copies in the calling funciton if necessary
	 * @param img    address of pointer to a bitmap that needs to be croped
	 * @param cropped_img address of pointer which will store the cropped image
	 * @param x      top left x coordinate
	 * @param y      top left y coordinate
	 * @param width  width of the desired cropped image
	 * @param height height of the desired cropped image
	 */
	void cropBitmap(Gdiplus::Bitmap** img, Gdiplus::Bitmap** cropped_img, int x, int y, int width, int height){
		// delete any current information at cropped pointer
		// pointer destination will be modified
		delete *cropped_img;
		*cropped_img = (*img)->Clone(x, y, width, height, PixelFormat32bppARGB);
	}

}