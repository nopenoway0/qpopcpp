#include <img_processing.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <settings_class.h>
#include <stdlib.h>
#include <ancillary_functions.h>

#include <boost/filesystem.hpp>

#define DEFAULT_RESCALE_Y 100
#define DEFAULT_RESCALE_X 100

// change image to same dimensions
float Img_Processing::compareImages(std::string img1, std::string img2){
	int* bucket1 = (int*) calloc(BUCKET_SIZE, sizeof(int));
	int* bucket2 = (int*) calloc(BUCKET_SIZE, sizeof(int));
	int bytes_to_read = 0;
	FILE* img1_loaded;
	FILE* img2_loaded;
	if(!(img1_loaded = fopen(img1.c_str(), "r"))) std::cerr << "Couldn't open image";
	if(!(img2_loaded = fopen(img2.c_str(), "r"))) std::cerr << "Couldn't open image"; // add exception?
	bytes_to_read = boost::filesystem::file_size(img1);
	Img_Processing::getBuckets(img1_loaded, bucket1, bytes_to_read - 1);
	fclose(img1_loaded);
	// practice clearing pointers
	//Print first 2kb of image 1
	/*for(int x = 0; x < BUCKET_SIZE; x++){
		printf("%02d ", bucket1[x]);
		if(x % 20 == 1 && x > 1) std::cout << "\n";
	}*/
	std::cout << "\n";
	bytes_to_read = boost::filesystem::file_size(img2);
	Img_Processing::getBuckets(img2_loaded, bucket2, bytes_to_read);
	//Print first 2kb of Image2
	/*for(int x = 0; x < BUCKET_SIZE; x++){
		printf("%02d ", bucket2[x]);
		if(x % 20 == 1 && x > 1) std::cout << "\n";
	}*/

	// Compare the buckets
	int difference = 0;
	int sum_diff = 0;
	for(int x = 0; x < BUCKET_SIZE; x++){
		difference = bucket1[x] - bucket2[x]; // average and compare to original?
		if(difference < 0) difference *= -1;
		sum_diff += difference;
	}
	printf("\nDifference between images: %%%.4f", ((float) sum_diff / (bytes_to_read * 256)) * 1000);
	fclose(img2_loaded);
	return 1.0;
}

void Img_Processing::getBuckets(FILE* img, int* store, int bytes_to_read){
	unsigned char* buffer = (unsigned char*) calloc(bytes_to_read, sizeof(char));
	fread(buffer, sizeof(char), bytes_to_read, img);
	for(int x = 0; x < bytes_to_read; x++){
		store[buffer[x]]++;
	}
}

float Img_Processing::compareMemoryImg(HBITMAP mem_img1, HBITMAP mem_img2){
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

struct result_data Img_Processing::compareMemoryImgSel(HBITMAP canvas, HBITMAP img, int x, int y, struct Scaling_Info* si_s){
	Gdiplus::Bitmap img1(canvas, NULL);
	std::vector<Gdiplus::Bitmap*> img2;
	struct result_data rdt;
	rdt.difference = NULL;
	//************************check canvas is bigger than img*********************************
	//****************************************************************************************
	float scale_factor_x = img1.GetWidth() / si_s->x_scaler;
	float scale_factor_y = img1.GetHeight() / si_s->y_scaler ;
	si_s->scaling_ratio = scale_factor_x;
	std::cout << "Scale factor " << scale_factor_y << scale_factor_x << " " << "\n";
	std::cout << "Snapshot dimensions after scale factor applied: " << img1.GetHeight() << "x" << img1.GetWidth() << "\n";
	std::vector<Gdiplus::Bitmap*> newmap;
	int count = si_s->num_images;
	std::cout << "number of images: " << count << std::endl;
	rdt.difference = (float*) calloc(sizeof(float), count);
	rdt.threshold = (float*) calloc(sizeof(float), count);
	//const CLSID codec = { 0x557cf406, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } };
	//newmap->Save(L"test.png", &codec, NULL);
	//exit(1);
	/*std::string file_name = "testf.png";

	const CLSID codec = { 0x557cf406, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } };
	int size = file_name.length();
	wchar_t name[size + 1];
	swprintf(name, size + 1, L"%s", file_name.c_str());
	int status = (*newmap).Save(name, &codec, NULL);
	if(status){
		std::cerr << "Error saving image: " << status << "\n";
		return 1;
	}*/
	float per_diff = 0;
	int rgb_diff[3] = {0};
	int temp;
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