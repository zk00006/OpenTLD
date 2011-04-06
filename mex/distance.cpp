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
#include "math.h"

// correlation
double ccorr(double *f1,double *f2,int numDim) {
	double f = 0;
	for (int i = 0; i<numDim; i++) {
		f += f1[i]*f2[i];
	}
	return f;
}

// correlation normalized
double ccorr_normed(double *f1,double *f2,int numDim) {
	double corr = 0;
	double norm1 = 0;
	double norm2 = 0;

	for (int i = 0; i<numDim; i++) {
		corr += f1[i]*f2[i];
		norm1 += f1[i]*f1[i];
		norm2 += f2[i]*f2[i];
	}
	// normalization to <0,1>
	return (corr / sqrt(norm1*norm2) + 1) / 2.0;
}

// euclidean distance
double euclidean(double *f1,double *f2,int numDim) {

	double sum = 0;
	for (int i = 0; i<numDim; i++) {
		sum += (f1[i]-f2[i])*(f1[i]-f2[i]);
	}
	return sqrt(sum);
}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

	if (nrhs == 0 || nrhs != 3) {
		mexPrintf("d = distance(x1,x2,flag\n");
		mexPrintf("flag = 1 : normalized correlation\n");
		mexPrintf("flag = 2 : euclidean distance\n");
		return;
	}

	double *x1 = mxGetPr(prhs[0]); int N1 = mxGetN(prhs[0]); int M1 = mxGetM(prhs[0]);
	double *x2 = mxGetPr(prhs[1]); int N2 = mxGetN(prhs[1]); int M2 = mxGetM(prhs[1]);

	if (M1 != M2) {
		mexPrintf("Wrong input!\n");
		return;
	}

	plhs[0] = mxCreateDoubleMatrix(N1, N2, mxREAL);
	double *resp = mxGetPr(plhs[0]);

	int flag = *mxGetPr(prhs[2]);

	switch (flag)
	{
	case 1 :
		for (int i = 0; i < N2; i++) {
			for (int ii = 0; ii < N1; ii++) {
				*resp++ = ccorr_normed(x1+ii*M1,x2+i*M1,M1);
			}
		}

		return;
	case 2 :

		for (int i = 0; i < N2; i++) {
			for (int ii = 0; ii < N1; ii++) {
				*resp++ = euclidean(x1+ii*M1,x2+i*M1,M1);
			}
		}

		return;
	}
}
