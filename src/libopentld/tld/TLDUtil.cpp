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

#include "TLDUtil.h"

#include "NormalizedPatch.h"
#include "DetectorCascade.h"

using namespace std;
using namespace cv;

namespace tld
{

void tldRectToPoints(Rect rect, CvPoint *p1, CvPoint *p2)
{
    p1->x = rect.x;
    p1->y = rect.y;
    p2->x = rect.x + rect.width;
    p2->y = rect.y + rect.height;
}

void tldBoundingBoxToPoints(int *bb, CvPoint *p1, CvPoint *p2)
{
    p1->x = bb[0];
    p1->y = bb[1];
    p2->x = bb[0] + bb[2];
    p2->y = bb[1] + bb[3];
}



//Returns mean-normalized patch, image must be greyscale
void tldNormalizeImg(const Mat &img, float *output)
{
    int size = TLD_PATCH_SIZE;

    Mat result;
    resize(img, result, cvSize(size, size)); //Default is bilinear

    float mean = 0;

    unsigned char *imgData = (unsigned char *)result.data;

    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 15; j++)
        {
            mean += imgData[j * result.step + i];
        }
    }

    mean /= size * size;


    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            output[j * 15 + i] = imgData[j * result.step + i] - mean;
        }
    }

}

CvRect tldBoundaryToRect(int *boundary)
{
    return Rect(boundary[0], boundary[1], boundary[2], boundary[3]);
}

void tldExtractSubImage(const Mat &img, Mat &subImage, CvRect rect)
{
    subImage = img(rect).clone();
}

void tldExtractSubImage(const Mat &img, Mat &subImage, int *boundary)
{
    tldExtractSubImage(img, subImage, tldBoundaryToRect(boundary));
}

void tldExtractNormalizedPatch(const Mat &img, int x, int y, int w, int h, float *output)
{
    Mat subImage;
    tldExtractSubImage(img, subImage, Rect(x, y, w, h));
    tldNormalizeImg(subImage, output);
}

//TODO: Rename
void tldExtractNormalizedPatchBB(const Mat &img, int *boundary, float *output)
{
    int x, y, w, h;
    tldExtractDimsFromArray(boundary, &x, &y, &w, &h);
    tldExtractNormalizedPatch(img, x, y, w, h, output);
}

void tldExtractNormalizedPatchRect(const Mat &img, Rect *rect, float *output)
{
    tldExtractNormalizedPatch(img, rect->x, rect->y, rect->width, rect->height, output);
}

float CalculateMean(float *value, int n)
{

    float sum = 0;

    for(int i = 0; i < n; i++)
        sum += value[i];

    return (sum / n);

}

float tldCalcVariance(float *value, int n)
{

    float mean = CalculateMean(value, n);
    float temp = 0;

    for(int i = 0; i < n; i++)
    {
        temp += (value[i] - mean) * (value[i] - mean) ;
    }

    return temp / n;

}


float tldBBOverlap(int *bb1, int *bb2)
{

    if(bb1[0] > bb2[0] + bb2[2])
    {
        return 0.0;
    }

    if(bb1[1] > bb2[1] + bb2[3])
    {
        return 0.0;
    }

    if(bb1[0] + bb1[2] < bb2[0])
    {
        return 0.0;
    }

    if(bb1[1] + bb1[3] < bb2[1])
    {
        return 0.0;
    }

    int colInt =  min(bb1[0] + bb1[2], bb2[0] + bb2[2]) - max(bb1[0], bb2[0]);
    int rowInt =  min(bb1[1] + bb1[3], bb2[1] + bb2[3]) - max(bb1[1], bb2[1]);

    int intersection = colInt * rowInt;
    int area1 = bb1[2] * bb1[3];
    int area2 = bb2[2] * bb2[3];
    return intersection / (float)(area1 + area2 - intersection);
}

void tldOverlapOne(int *windows, int numWindows, int index, vector<int> * indices, float *overlap)
{

    for(size_t i = 0; i < indices->size(); i++)
    {

        overlap[i] = tldBBOverlap(&windows[TLD_WINDOW_SIZE * index], &windows[TLD_WINDOW_SIZE * indices->at(i)]);
    }

}

float tldOverlapRectRect(Rect r1, Rect r2)
{
    int bb1[4];
    int bb2[4];
    tldRectToArray<int>(r1, bb1);
    tldRectToArray<int>(r2, bb2);
    return tldBBOverlap(bb1, bb2);

}

Rect *tldCopyRect(Rect *r)
{
    Rect *r2 = new Rect();
    r2->x = r->x;
    r2->y = r->y;
    r2->width = r->width;
    r2->height = r->height;
    return r2;
}

void tldOverlapRect(int *windows, int numWindows, Rect *boundary, float *overlap)
{
    int bb[4];
    bb[0] = boundary->x;
    bb[1] = boundary->y;
    bb[2] = boundary->width;
    bb[3] = boundary->height;

    tldOverlap(windows, numWindows, bb, overlap);
}

void tldOverlap(int *windows, int numWindows, int *boundary, float *overlap)
{

    for(int i = 0; i < numWindows; i++)
    {

        overlap[i] = tldBBOverlap(boundary, &windows[TLD_WINDOW_SIZE * i]);
    }

}




bool tldSortByOverlapDesc(pair<int, float> bb1 , pair<int, float> bb2)
{
    return bb1.second > bb2.second;
}

//Checks whether bb1 is completely inside bb2
int tldIsInside(int *bb1, int *bb2)
{

    if(bb1[0] > bb2[0] && bb1[1] > bb2[1] && bb1[0] + bb1[2] < bb2[0] + bb2[2] && bb1[1] + bb1[3] < bb2[1] + bb2[3])
    {
        return 1;
    }
    else return 0;

}
} /* End Namespace */
