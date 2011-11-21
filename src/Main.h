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
