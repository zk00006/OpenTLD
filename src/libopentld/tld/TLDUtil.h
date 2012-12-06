/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/*
 * util.h
 *
 *  Created on: 30.05.2011
 *      Author: Georg Nebehay
 */

#ifndef TLDUTIL_H_
#define TLDUTIL_H_

#include <utility>

#include <opencv/cv.h>

namespace tld
{

template <class T1, class T2>
void tldConvertBB(T1 *src, T2 *dest)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    dest[3] = src[3];
}

template <class T>
void tldCopyBB(T *src, T *dest)
{
    tldConvertBB<T, T>(src, dest);
}

template <class T>
void tldCopyBoundaryToArray(T x, T y, T width, T height, T *array)
{
    array[0] = x;
    array[1] = y;
    array[2] = width;
    array[3] = height;
}

template <class T>
void tldExtractDimsFromArray(T *boundary, T *x, T *y, T *width, T *height)
{
    *x = boundary[0];
    *y = boundary[1];
    *width = boundary[2];
    *height = boundary[3];
}

template <class T>
void tldRectToArray(cv::Rect rect, T *boundary)
{
    boundary[0] = rect.x;
    boundary[1] = rect.y;
    boundary[2] = rect.width;
    boundary[3] = rect.height;
}

template <class T>
cv::Rect tldArrayToRect(T *boundary)
{
    cv::Rect rect;
    rect.x = boundary[0];
    rect.y = boundary[1];
    rect.width = boundary[2];
    rect.height = boundary[3];

    return rect;
}


int tldIsInside(int *bb1, int *bb2);
void tldRectToPoints(CvRect rect, CvPoint *p1, CvPoint *p2);
void tldBoundingBoxToPoints(int *bb, CvPoint *p1, CvPoint *p2);

void tldNormalizeImg(const cv::Mat &img, float *result, int size);

void tldExtractNormalizedPatch(const cv::Mat &img, int x, int y, int w, int h, float *output);
void tldExtractNormalizedPatchBB(const cv::Mat &img, int *boundary, float *output);
void tldExtractNormalizedPatchRect(const cv::Mat &img, cv::Rect *rect, float *output);
void tldExtractSubImage(const cv::Mat &img, cv::Mat &subImage, int *boundary);
void tldExtractSubImage(const cv::Mat &img, cv::Mat &subImage, int x, int y, int w, int h);

float tldCalcMean(float *value, int n);
float tldCalcVariance(float *value, int n);

bool tldSortByOverlapDesc(std::pair<int, float> bb1 , std::pair<int, float> bb2);
cv::Rect *tldCopyRect(cv::Rect *r);

//TODO: Change function names
float tldOverlapRectRect(cv::Rect r1, cv::Rect r2);
void tldOverlapOne(int *windows, int numWindows, int index, std::vector<int> * indices, float *overlap);
void tldOverlap(int *windows, int numWindows, int *boundary, float *overlap);
void tldOverlapRect(int *windows, int numWindows, cv::Rect *boundary, float *overlap);

float tldCalcVariance(float *value, int n);

#endif /* UTIL_H_ */

} /* End Namespace */
