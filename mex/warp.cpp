// Copyright 2011 Zdenek Kalal
//
// This file is part of TLD.
// 
// TLD is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// TLD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with TLD.  If not, see <http://www.gnu.org/licenses/>.

#include "mex.h"
#include <math.h>

#ifndef NAN
#define NAN 0/0
#endif

#ifndef M_PI
#define M_PI 3.14159265358979L
#endif
// rowwise access
#define coord(x, y, width, height) (y+x*height)
#define nextrow(tmp, width, height) ((tmp)+1)
#define nextcol(tmp, width, height) ((tmp)+height)
#define nextr_c(tmp, width, height) ((tmp)+height+1)

#define M(r, c) H[c*3+r]

/* Warps image of size w x h, using affine transformation matrix (2x2 part) 
   and offset (center of warping) ofsx, ofsy. Result is the region of size 
   defined with roi. */
void warp_image_roi(unsigned char *image, int w, int h, double *H,
                    double xmin, double xmax, double ymin, double ymax,
                    double fill, double *result)
{
   double curx, cury, curz, wx, wy, wz, ox, oy, oz;
   int x, y;
   unsigned char *tmp;
   double *output=result, i, j, xx, yy;
   /* precalulate necessary constant with respect to i,j offset 
      translation, H is column oriented (transposed) */   
   ox = M(0,2);
   oy = M(1,2);
   oz = M(2,2);

   yy = ymin;
   for (j=0; j<(int)(ymax-ymin+1); j++)
   {
      /* calculate x, y for current row */
      curx = M(0,1)*yy + ox;
      cury = M(1,1)*yy + oy;
      curz = M(2,1)*yy + oz;
      xx = xmin; 
      yy = yy + 1;
      for (i=0; i<(int)(xmax-xmin+1); i++)
      {
         /* calculate x, y in current column */
         wx = M(0,0)*xx + curx;
         wy = M(1,0)*xx + cury;
         wz = M(2,0)*xx + curz;
//       printf("%g %g, %g %g %g\n", xx, yy, wx, wy, wz);
         wx /= wz; wy /= wz;
         xx = xx + 1;
         
         x = (int)floor(wx);
         y = (int)floor(wy);

         if (x>=0 && y>=0)
         {
            wx -= x; wy -= y; 
            if (x+1==w && wx==1)
               x--;
            if (y+1==h && wy==1)
               y--;
            if ((x+1)<w && (y+1)<h)
            {
               tmp = &image[coord(x,y,w,h)];
               /* image[x,y]*(1-wx)*(1-wy) + image[x+1,y]*wx*(1-wy) +
                  image[x,y+1]*(1-wx)*wy + image[x+1,y+1]*wx*wy */
               *output++ = 
                  (*(tmp) * (1-wx) + *nextcol(tmp, w, h) * wx) * (1-wy) +
                  (*nextrow(tmp,w,h) * (1-wx) + *nextr_c(tmp,w,h) * wx) * wy;
            } else 
               *output++ = fill;
         } else 
            *output++ = fill;
      }
   }
}

mxArray *to_matlab(const double *image, int num_cols, int num_rows)
{
   // convert to matlab's column based representation
   int i, j;
   mxArray *result;
   const double* s_ptr = image;
   double* d_ptr,* data;
   result = mxCreateDoubleMatrix(num_rows, num_cols, mxREAL);
   data = (double *)mxGetData(result);
   for (i=0;i<num_rows;i++)
   {
      d_ptr = &data[i];
      for (j=0; j<num_cols; j++, d_ptr+=num_rows, s_ptr++)
         (*d_ptr) = (*s_ptr);
   }
   return result;
}

void mexFunction (int nlhs, mxArray *plhs [], int nrhs, const mxArray *prhs [])
{
   int w, h;
   unsigned char *im;
   double *result;
   double *H = 0;
   double xmin, xmax, ymin, ymax, fill;
   if (nrhs>0)
   {
	  w = mxGetN(prhs[0]);
	  h = mxGetM(prhs[0]);
	  im = (unsigned char*) mxGetPr(prhs[0]);
      //from_matlab(prhs[0], &im, &w, &h);
      if (nrhs>1)
         H = (double *)mxGetData(prhs[1]);

      if (nrhs>2 && mxGetM(prhs[2])>0)
      {
         double *B;
         B = (double*)mxGetData(prhs[2]);
         xmin = (*B++); xmax = (*B++);
         ymin = (*B++); ymax = (*B++);
      } else {
         xmin = ymin = 0;
         xmax = w-1; ymax = h-1;
      }
      if (nrhs>3)
         fill=0;
      else
         fill=0;
      result=new double[((int)(xmax-xmin+1)*(int)(ymax-ymin+1))];
      {
         warp_image_roi(im, w, h, H, xmin, xmax, ymin, ymax, fill, result);
      }
      
      plhs[0]=to_matlab(result, (int)(xmax-xmin+1), (int)(ymax-ymin+1));
      delete [] result;
   }
}
