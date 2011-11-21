/*
 * export.h
 *
 *  Created on: Nov 18, 2011
 *      Author: georg
 */

#ifndef EXPORT_H_
#define EXPORT_H_

#include "TLD.h"

using namespace tld;

void tldWriteToFile(TLD * tld, const char * path);
void tldReadFromFile(TLD * tld, const char * path);


#endif /* EXPORT_H_ */
