 Introduction
==============

This is a C++ implementation of OpenTLD that was originally published in MATLAB by Zdenek Kalal. OpenTLD is used for tracking objects in video streams. What makes this algorithm outstanding is that it does not make use of any training data. This implementation is based solely on open source libraries, meaning that you do not need any commercial products to compile or run it.

The easiest way to get started is to download the precompiled binaries that are available for Windows and Ubuntu 10.04. You can get a .deb package here:
http://dl.dropbox.com/u/8715460/tld.deb
and a windows installer file here:
http://dl.dropbox.com/u/8715460/tld.zip

If you have a webcam attached to your PC, you can simply execute tld (on Ubuntu) or tld.exe (on Windows) in order to
try it out. For other configuration options, please see below.

 Usage
=======

Keyboard shortcuts
-------------------

q...quit  
b...remember current frame as background model / clear background  
c...clear model and stop tracking  
l...toggle learning  
a...toggle alternating mode (if true, detector is switched off when tracker is available)  
e...export model to file specified in configuration parameter "modelExportFile"  
i...import model from file specified in configuration parameter "modelPath"  
r...clear model, let user reinit tracking

command line options
--------------------

usage: tld [option arguments] [CONFIGFILE]  
option arguments:  
[-a _startFrameNumber_] video starts at the frameNumber _startFrameNumber_  
[-b _x,y,w,h_] Initial bounding box  
[-c] shows color images instead of greyscale  
[-d _device_] select input device: _device_=(IMGS|CAM|VID)  
	IMGS: capture from images
	CAM: capture from connected camera
	VID: capture from a video
[-e _path_] export model after run to _path_  
[-f] shows foreground  
[-i _path_] _path_ to the images or to the video  
[-h] shows help  
[-j _number_] specifies the _number_ of the last frames which are considered by the trajectory; 0 disables the trajectory  
[-m _path_] if specified load a model from _path_. An initialBoundingBox must be specified or selectManually must be true.  
[-p _path_] prints results into the file _path_  
[-q] open QT-Config GUI  
[-s] if set, user can select initial bounding box  
[-t _theta_] threshold for determining positive results  
[-z _lastFrameNumber_] video ends at the frameNumber _lastFrameNumber_.
	If _lastFrameNumber_ is 0 or the optaion argument isn't specified means
	take all frames are taken.  
arguments:  
[CONFIG_FILE] to the config file

 Compiling
===========

Dependencies:

* OpenCV & OpenCV highgui >= 2.0
* CMake >= 2.6
* QtGui4 & QtCore4 >= 4.4 (optional)
* NSIS (optional, Windows)

Use CMake to build the project. You can use "cmake-gui", if you need a graphical user interface.

* Specify the source path (root path of the dictionary) and the binary path (where to build the program, out
  of source buid recommended)
* Configure
* Select compiler
* Define the install path and other options
* Generate
* Windows: go to the binary directory and build the solutions you want (!!!You have to compile in RELEASE mode!!!):
* tld: build the project
	* INSTALL: install the project
	* PACKAGE: create a NSIS-installer (NSIS required)
          Linux: go with the terminal to the build directory
	* make: build the project
	* make install all: install the project
	* make package: generates a package (.tgz file if GENERATE_DEB_PACKAGE=false, .deb file if GENERATE_DEB_PACKAGE=true)

Options:

* BUILD_WITH_QT: build project with a Qt-settings-dialog (Qt must be installed)
* GENERATE_DEB_PACKAGE(Linux): build a Debian (.deb) packages
