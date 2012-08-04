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
 * NormalizedPatch.h
 *
 *  Created on: Nov 16, 2011
 *      Author: Georg Nebehay
 */

#ifndef NORMALIZEDPATCH_H_
#define NORMALIZEDPATCH_H_

#define TLD_PATCH_SIZE 15

namespace tld
{

class NormalizedPatch
{
public:
    float values[TLD_PATCH_SIZE *TLD_PATCH_SIZE];
    bool positive;
};

} /* namespace tld */
#endif /* NORMALIZEDPATCH_H_ */
