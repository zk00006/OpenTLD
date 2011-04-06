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

#include <stdio.h>
#include "mex.h"
#include "tld.h"

/* Integral image^2, requires uint8 at the input*/
/* example: out = ii(in) */

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

	if (nrhs != 1 && nrhs != 2) {
		mexPrintf("ii2: wrong input.\n");
		return;
	}

	// Input
	unsigned char *in = (unsigned char *) mxGetData(prhs[0]);
	int imH = mxGetM(prhs[0]);
	int imW = mxGetN(prhs[0]);

	// Output
	double *ii;
	if (nrhs==1) {
		plhs[0] = mxCreateDoubleMatrix(imH, imW, mxREAL);
		ii = mxGetPr(plhs[0]);
	} else {
		ii = mxGetPr(prhs[1]);
	}

	iimg2(in,ii,imH,imW);
}
