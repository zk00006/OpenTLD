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
#include <stdio.h>
#include <stdlib.h>

#include "TLD.h"

using namespace tld;

typedef struct {
	int index;
	int P;
	int N;
} TldExportEntry;

void tldWriteToFile(TLD * tld, const char * path) {
	NNClassifier * nn = tld->detectorCascade->nnClassifier;
	EnsembleClassifier* ec = tld->detectorCascade->ensembleClassifier;

	FILE * file = fopen(path, "w");
	fprintf(file,"#Tld ModelExport\n");
	fprintf(file,"%d #width\n", tld->detectorCascade->objWidth);
	fprintf(file,"%d #height\n", tld->detectorCascade->objHeight);
	fprintf(file,"%f #min_var\n", tld->detectorCascade->varianceFilter->minVar);
	fprintf(file,"%d #Positive Sample Size\n", nn->truePositives->size());



	for(size_t s = 0; s < nn->truePositives->size();s++) {
		float * imageData =nn->truePositives->at(s).values;
		for(int i = 0; i < TLD_PATCH_SIZE; i++) {
			for(int j = 0; j < TLD_PATCH_SIZE; j++) {
				fprintf(file, "%f ", imageData[i*TLD_PATCH_SIZE+j]);
			}
			fprintf(file, "\n");
		}
	}

	fprintf(file,"%d #Negative Sample Size\n", nn->falsePositives->size());

	for(size_t s = 0; s < nn->falsePositives->size();s++) {
		float * imageData = nn->falsePositives->at(s).values;
		for(int i = 0; i < TLD_PATCH_SIZE; i++) {
			for(int j = 0; j < TLD_PATCH_SIZE; j++) {
				fprintf(file, "%f ", imageData[i*TLD_PATCH_SIZE+j]);
			}
			fprintf(file, "\n");
		}
	}

	fprintf(file,"%d #numtrees\n", ec->numTrees);
	fprintf(file,"%d #numFeatures\n", ec->numFeatures);
	for(int i = 0; i < ec->numTrees; i++) {
		fprintf(file, "#Tree %d\n", i);

		for(int j = 0; j < ec->numFeatures; j++) {
			float * features = ec->features + 4*ec->numFeatures*i + 4*j;
			fprintf(file,"%f %f %f %f # Feature %d\n", features[0], features[1], features[2], features[3], j);
		}

		//Collect indices
		vector<TldExportEntry> list;

		for(int index = 0; index < pow(2.0f, ec->numFeatures); index++) {
			int p = ec->positives[i * ec->numIndices + index];
			if(p != 0) {
				TldExportEntry entry;
				entry.index = index;
				entry.P = p;
				entry.N = ec->negatives[i * ec->numIndices + index];
				list.push_back(entry);
			}
		}

		fprintf(file,"%d #numLeaves\n", list.size());
		for(size_t j = 0; j < list.size(); j++) {
			TldExportEntry entry = list.at(j);
			fprintf(file,"%d %d %d\n", entry.index, entry.P, entry.N);
		}
	}

	fclose(file);

}

void tldReadFromFile(TLD * tld, const char * path) {
	tld->release();

	NNClassifier * nn = tld->detectorCascade->nnClassifier;
	EnsembleClassifier* ec = tld->detectorCascade->ensembleClassifier;

	FILE * file = fopen(path, "r");

	if(file == NULL) {
		printf("Error: Model not found: %s\n", path);
		exit(1);
	}

	int MAX_LEN=255;
	char str_buf[255];
	fgets(str_buf, MAX_LEN, file); /*Skip line*/

	fscanf(file,"%d \n", &tld->detectorCascade->objWidth);
	fgets(str_buf, MAX_LEN, file); /*Skip rest of line*/
	fscanf(file,"%d \n", &tld->detectorCascade->objHeight);
	fgets(str_buf, MAX_LEN, file); /*Skip rest of line*/

	fscanf(file,"%f \n", &tld->detectorCascade->varianceFilter->minVar);
	fgets(str_buf, MAX_LEN, file); /*Skip rest of line*/

	int numPositivePatches;
	fscanf(file, "%d \n", &numPositivePatches);
	fgets(str_buf, MAX_LEN, file); /*Skip line*/


	for(int s = 0; s < numPositivePatches; s++) {
		NormalizedPatch patch;

		for(int i = 0; i < 15; i++) { //Do 15 times

			fgets(str_buf, MAX_LEN, file); /*Read sample*/

			char * pch;
			pch = strtok (str_buf," ");
			int j = 0;
			while (pch != NULL)
			{
				float val = atof(pch);
				patch.values[i*TLD_PATCH_SIZE+j] = val;

				pch = strtok (NULL, " ");

				j++;
			}
		}

		nn->truePositives->push_back(patch);
	}

	int numNegativePatches;
	fscanf(file, "%d \n", &numNegativePatches);
	fgets(str_buf, MAX_LEN, file); /*Skip line*/


	for(int s = 0; s < numNegativePatches; s++) {
		NormalizedPatch patch;
		for(int i = 0; i < 15; i++) { //Do 15 times

			fgets(str_buf, MAX_LEN, file); /*Read sample*/

			char * pch;
			pch = strtok (str_buf," ");
			int j = 0;
			while (pch != NULL)
			{
				float val = atof(pch);
				patch.values[i*TLD_PATCH_SIZE+j] = val;

				pch = strtok (NULL, " ");

				j++;
			}
		}

		nn->falsePositives->push_back(patch);
	}

	fscanf(file,"%d \n", &ec->numTrees);
	tld->detectorCascade->numTrees = ec->numTrees;
	fgets(str_buf, MAX_LEN, file); /*Skip rest of line*/

	fscanf(file,"%d \n", &ec->numFeatures);
	tld->detectorCascade->numFeatures = ec->numFeatures;
	fgets(str_buf, MAX_LEN, file); /*Skip rest of line*/

	int size = 2 * 2 * ec->numFeatures * ec->numTrees;
	ec->features = new float[size];
	ec->numIndices = pow(2.0f, ec->numFeatures);
	ec->initPosteriors();

	for(int i = 0; i < ec->numTrees; i++) {
		fgets(str_buf, MAX_LEN, file); /*Skip line*/

		for(int j = 0; j < ec->numFeatures; j++) {
			float * features = ec->features + 4*ec->numFeatures*i + 4*j;
			fscanf(file, "%f %f %f %f",&features[0], &features[1], &features[2], &features[3]);
			fgets(str_buf, MAX_LEN, file); /*Skip rest of line*/
		}

		/* read number of leaves*/
		int numLeaves;
		fscanf(file,"%d \n", &numLeaves);
		fgets(str_buf, MAX_LEN, file); /*Skip rest of line*/

		for(int j = 0; j < numLeaves; j++) {
			TldExportEntry entry;
			fscanf(file,"%d %d %d \n", &entry.index, &entry.P, &entry.N);
			ec->updatePosterior(i, entry.index, 1, entry.P);
			ec->updatePosterior(i, entry.index, 0, entry.N);
		}
	}

	tld->detectorCascade->initWindowsAndScales();
	tld->detectorCascade->initWindowOffsets();

	tld->detectorCascade->propagateMembers();

	tld->detectorCascade->initialised = true;

	ec->initFeatureOffsets();

}
