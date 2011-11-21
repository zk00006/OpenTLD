/*
 * NormalizedPatch.h
 *
 *  Created on: Nov 16, 2011
 *      Author: georg
 */

#ifndef NORMALIZEDPATCH_H_
#define NORMALIZEDPATCH_H_

#define TLD_PATCH_SIZE 15

namespace tld {

class NormalizedPatch {
public:
	float values[TLD_PATCH_SIZE*TLD_PATCH_SIZE];
	bool positive;
};

} /* namespace tld */
#endif /* NORMALIZEDPATCH_H_ */
