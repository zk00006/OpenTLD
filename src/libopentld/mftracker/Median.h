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

/**
 * @file median.h
 *
 *  Created on: 22.03.2011
 *     @author: bernd
 * @brief
 */

#ifndef MEDIAN_H_
#define MEDIAN_H_

/**
 * Returns median of the array. Changes array!
 * @param arr the array
 * @pram n length of array
 */
float getMedian(float arr[], int n);

/**
 * Calculates Median of the array. Don't change array(makes copy).
 * @param arr the array
 * @pram n length of array
 */
float getMedianUnmanaged(float arr[], int n);

#endif /* MEDIAN_H_ */
