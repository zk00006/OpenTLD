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

void iimg(unsigned char *in, double *ii, int imH, int imW) {

	double *prev_line = ii;
	double s;

	*ii++ = *in++;

	for (int x = 1; x < imH; x++) {
		*ii = *in + *(ii-1);
		ii++; in++;
	}

	for (int y = 1; y < imW; y++) {
		s = 0;
		for (int x = 0; x < imH; x++) {
			s += *in;
			*ii = s + *prev_line;
			ii++; in++; prev_line++;
		}
	}
}


void iimg2(unsigned char *in, double *ii2, int imH, int imW) {

	double *prev_line = ii2;
	double s;

	*ii2++ = (*in)*(*in); in++;

	for (int x = 1; x < imH; x++) {
		*ii2 = (*in)*(*in) + *(ii2-1);
		ii2++; in++;
	}

	for (int y = 1; y < imW; y++) {
		s = 0;
		for (int x = 0; x < imH; x++) {
			s += *in * *in;
			*ii2 = s + *prev_line;
			ii2++; in++; prev_line++;
		}
	}

}


double bbox_var_offset(double *ii,double *ii2, int *off) {
	// off[0-3] corners of bbox, off[4] area
	double mX  = (ii[off[3]] - ii[off[2]] - ii[off[1]] + ii[off[0]]) / (double) off[4];
	double mX2 = (ii2[off[3]] - ii2[off[2]] - ii2[off[1]] + ii2[off[0]]) / (double) off[4];
	return mX2 - mX*mX;
}

