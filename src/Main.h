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
 * main.h
 *
 *  Created on: Nov 18, 2011
 *      Author: georg
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "TLD.h"
#include "imAcq.h"
#include "gui.h"

using namespace tld;

enum Retval {
	PROGRAM_EXIT = 0,
	SUCCESS = 1
};

class Main {
public:
	TLD * tld;
	ImAcq * imAcq;
	Gui * gui;

	bool showOutput;
	const char * printResults;
	bool saveOutput;
	const char * saveDir;
	double threshold;
	bool showForeground;
	bool showNotConfident;
	bool showColorImage;
	bool alternating;
	bool learningEnabled;
	bool selectManually;
	int * initialBB;
	bool reinit;
	bool exportModelAfterRun;
	bool loadModel;
	const char * modelPath;
	const char * modelExportFile;

	Main() {
		tld = new TLD();
		showOutput = 1;
		printResults = NULL;
		saveOutput = 0;
		saveDir = ".";
		threshold = 0.5;
		showForeground = 0;
		showColorImage = 0;

		alternating = 0;

		learningEnabled = 1;

		selectManually = 0;

		initialBB = NULL;
		showNotConfident = true;

		reinit = 0;

		loadModel = false;

		exportModelAfterRun = false;
		modelExportFile = "model";
	}

	~Main() {
		delete tld;
		imAcqFree(imAcq);
	}

	void doWork();
};

#endif /* MAIN_H_ */
