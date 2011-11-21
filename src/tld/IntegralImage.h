/*
 * IntegralImage.h
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#ifndef INTEGRALIMAGE_H_
#define INTEGRALIMAGE_H_

#include <cv.h>

using namespace cv;

namespace tld {

template <class T>
class IntegralImage {
public:
	T * data; /* Array containg the entries for the integral image in row-first manner. Of size width*height. Allocated by tldAllocIntImg */
		/* width, height: Dimensions of integral image.*/
	int width;
	int height;

	IntegralImage(Size size) {
		data = new T[size.width*size.height];
	}

	virtual ~IntegralImage() {
		delete[] data;
	}

	void calcIntImg(Mat img, bool squared = false)
	{
		unsigned char *input = (unsigned char*)(img.data);
		T *output = data;
		for(int i = 0;i < img.cols;i++){
			for(int j = 0;j < img.rows;j++){
				T A = (i > 0) ? output[img.cols * j + i - 1] : 0;
				T B = (j > 0) ? output[img.cols * (j - 1) + i] : 0;
				T C = (j > 0 && i > 0) ? output[img.cols * (j - 1) + i - 1] : 0;
				T value = input[img.step * j + i];
				if(squared) {
					value = value*value;
				}
				output[img.cols * j + i] = A + B - C + value;
			}
		}

	}
};


} /* namespace tld */
#endif /* INTEGRALIMAGE_H_ */
