#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include <ostream>
#include <windows.h>
#include <gdiplus.h>
#include <ancillary_functions.h>
#define BUCKET_SIZE 256
#define DEFAULT_THRESHOLD 3 // default difference in picture, greater than this the picture doesn't match

namespace Img_Processing{
		using namespace std;
		using namespace Ancillary_Function;
		
		float compareImages(string img1, string img2);
		void setThreshold(float newValue);
		float compareMemoryImg(HBITMAP mem_img1, HBITMAP mem_img2);
		float compareMemoryImg(Gdiplus::Bitmap* img1, Gdiplus::Bitmap* img2);
		struct result_data compareMemoryImgSel(HBITMAP canvas, HBITMAP img, int x, int y, struct Scaling_Info* si_s);
		void getBuckets(FILE* img, int* store, int bytes_to_read);
		void cropBitmap(Gdiplus::Bitmap** img, int x, int y, int width, int height);
}
#endif