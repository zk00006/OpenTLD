 Introduction
==============

This is a C++ implementation of OpenTLD that was originally published in MATLAB by Zdenek Kalal. OpenTLD is used for tracking objects in video streams. What makes this algorithm outstanding is that it does not make use of any training data. This implementation is based solely on open source libraries, meaning that you do not need any commercial products to compile or run it.

The easiest way to get started is to download the precompiled binaries that are available for Windows and Ubuntu 10.04. You can get a .deb package here:
http://dl.dropbox.com/u/8715460/tld.deb
and a windows installer file here:
http://dl.dropbox.com/u/8715460/tld.zip

If you have a webcam attached to your PC, you can simply execute opentld (on Ubuntu) or opentld.exe (on Windows) in order to
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

__usage__  
`opentld [option arguments] [arguments]`

__option arguments__  
`[-a <startFrameNumber>]` video starts at the frameNumber _startFrameNumber_  
`[-b <x,y,w,h>]` Initial bounding box  
`[-d <device>]` select input device: _device_=(IMGS|CAM|VID)  
	IMGS: capture from images
	CAM: capture from connected camera
	VID: capture from a video
`[-e <path>]` export model after run to _path_  
`[-f]` shows foreground  
`[-i <path>]` _path_ to the images or to the video. 
`[-h]` shows help  
`[-m <path>]` if specified load a model from _path_. An initialBoundingBox must be specified or selectManually must be true.  
`[-n <number>]` Specifies the video device to use (defaults to 0). Useful to select a different camera when multiple cameras are connected.   
`[-p path]` prints results into the file _path_  
`[-s]` if set, user can select initial bounding box  
`[-t <theta>]` threshold for determining positive results  
`[-z <lastFrameNumber>]` video ends at the frameNumber _lastFrameNumber_.
	If _lastFrameNumber_ is 0 or the option argument isn't specified means
	all frames are taken.  

__arguments__  
`[CONFIG_FILE]` path to config file

 Building
=========

Dependencies
------------

* OpenCV & OpenCV highgui >= 2.0
* CMake >= 2.6

Compiling
---------

Use CMake to build the project. You can use "cmake-gui", if you need a graphical user interface.

* CMake
	* Specify the source path (root path of the dictionary) and the binary path (where to build the program, out
	  of source build recommended)
	* Configure
	* Select compiler
	* Define the install path and other options
	* Generate
* Windows: go to the binary directory and build the solutions you want (You have to compile in RELEASE mode):
	* opentld: build the project
	* INSTALL: install the project
* Linux: go with the terminal to the build directory
	* make: build the project
* Mac
	* brew install python
	* brew install gfortran
	* easy_install numpy
	* brew install cmake
	* brew install opencv
	* mkdir build
	* cd build
	* cmake ..
	* cd ..
	* make
	* ./opentld

Generate Package
----------------

* Debian package
	* Navigate with the terminal into the root dictionary of OpenTLD (OpenTLD/)
	* Type `debuild -us -uc`
	* (For now it's not possible to `make clean` therefore there are lots of remaining junk files in the source tree after the package-build)

