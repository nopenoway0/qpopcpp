#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include <ostream>
#include <windows.h>
#include <gdiplus.h>
#include <ancillary_functions.h>
#define BUCKET_SIZE 256
#define DEFAULT_THRESHOLD 3 // default difference in picture, greater than this the picture doesn't match

class Img_Processing{
	public:
		static float compareImages(std::string img1, std::string img2);
		static void setThreshold(float newValue);
		static float compareMemoryImg(HBITMAP mem_img1, HBITMAP mem_img2);
		static struct result_data compareMemoryImgSel(HBITMAP canvas, HBITMAP img, int x, int y, struct Scaling_Info* si_s);
	private:
		static void getBuckets(FILE* img, int* store, int bytes_to_read);
};
#endif