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

	float compareImages(std::string img1, std::string img2){
		int sum_diff, temp = 0;
		float per_diff = 0;
		int rgb_diff[3] = {0};
		FILE* img1_loaded;
		FILE* img2_loaded;
		if(!(img1_loaded = fopen(img1.c_str(), "r"))) std::cerr << "Couldn't open image";
		if(!(img2_loaded = fopen(img2.c_str(), "r"))) std::cerr << "Couldn't open image"; // add exception?

		Gdiplus::Bitmap* img1_bmp = new Gdiplus::Bitmap(std::wstring(img1.begin(), img1.end()).c_str(), NULL);
		Gdiplus::Bitmap* img2_bmp = new Gdiplus::Bitmap(std::wstring(img2.begin(), img2.end()).c_str(), NULL);
		img1_bmp->SetResolution(DEFAULT_RESCALE_X, DEFAULT_RESCALE_Y);
		img2_bmp->SetResolution(DEFAULT_RESCALE_X, DEFAULT_RESCALE_Y);
		Gdiplus::Color c1;
		Gdiplus::Color c2;

		for(int x = 0; x < DEFAULT_RESCALE_X; x++){
			for(int y = 0; y < DEFAULT_RESCALE_Y; y++){
				img1_bmp->GetPixel(x, y, &c1);
				img2_bmp->GetPixel(x, y, &c2);
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
		for(int x = 0; x < 3; x++){
			per_diff += rgb_diff[x];
		}
		per_diff = per_diff / (DEFAULT_RESCALE_Y * DEFAULT_RESCALE_X);

		// clean up
		DeleteObject(img1_bmp);
		DeleteObject(img2_bmp);
		return per_diff;
	}

	float compareMemoryImg(HBITMAP mem_img1, HBITMAP mem_img2){
		Gdiplus::Bitmap img1(mem_img1, NULL);
		Gdiplus::Bitmap img2(mem_img2, NULL);
		float per_diff = 0;
		int rgb_diff[3] = {0};
		int temp;
		// Gives Resolution of Image
		//img1.GetWidth() img1.GetHeight() << ;
		img1.SetResolution(DEFAULT_RESCALE_X, DEFAULT_RESCALE_Y);
		img2.SetResolution(DEFAULT_RESCALE_X, DEFAULT_RESCALE_Y);
		Gdiplus::Color c1;
		Gdiplus::Color c2;
		for(int x = 0; x < DEFAULT_RESCALE_X; x++){
			for(int y = 0; y < DEFAULT_RESCALE_Y; y++){
				img1.GetPixel(x, y, &c1);
				img2.GetPixel(x, y, &c2);
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
		for(int x = 0; x < 3; x++){
			per_diff += rgb_diff[x];
		}
		per_diff = per_diff / (DEFAULT_RESCALE_Y * DEFAULT_RESCALE_X);
		return per_diff;
	}

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
	 * @return      the overall difference as a decimal which is <= 1
	 */
	float compareMemoryImg(Gdiplus::Bitmap* img1, Gdiplus::Bitmap* img2){
		// compare bitmaps
		float difference, per_diff = 0;
		int height, width;
		height = img1->GetHeight();
		width = img1->GetWidth();
		if(height > img2->GetHeight()) height = img2->GetHeight();
		if(width > img2->GetWidth()) width = img2->GetWidth();

		Gdiplus::BitmapData* bitmapData1 = new Gdiplus::BitmapData;
		Gdiplus::BitmapData* bitmapData2 = new Gdiplus::BitmapData;
		Gdiplus::Color c1;
		Gdiplus::Color c2;

		Gdiplus::Rect rect(0,0, width, height);
		img1->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData1);
		img2->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData2);

		int current_pixel_place;
		unsigned char* pixels_1 = (unsigned char*) bitmapData1->Scan0;
		unsigned char* pixels_2 = (unsigned char*) bitmapData2->Scan0;

		for(int x = 0; x < height; x++){
			for(int y = 0; y < width; y++){
				current_pixel_place = x * bitmapData1->Stride / 4 + y*4;
				for(int z = 0; z < 2; z ++)
				{
					difference = pixels_1[current_pixel_place + z] - pixels_2[current_pixel_place + z];
					if(difference < 0) difference *= -1;
					per_diff += difference;
				}
			}
		}

		img1->UnlockBits(bitmapData1);
		img2->UnlockBits(bitmapData2);
		delete bitmapData1;
		delete bitmapData2;
		DeleteObject(bitmapData1);
		DeleteObject(bitmapData2);
		return (per_diff / (width * height * 765));
	}

	void cropBitmap(Gdiplus::Bitmap** img, int x, int y, int width, int height){
		Gdiplus::Bitmap* temp = (*img)->Clone(x, y, width, height, PixelFormat32bppARGB);
		delete *img;
		*img = temp;
	}

}