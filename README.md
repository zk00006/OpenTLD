# Introduction

This is a C++ implementation of OpenTLD that was originally published in MATLAB by Zdenek Kalal. OpenTLD is used for tracking objects in video streams. What makes this algorithm outstanding is that it does not make use of any training data. This implementation is based solely on open source libraries, meaning that you do not need any commercial products to compile or run it.

The easiest way to get started is to download the precompiled [binaries](https://github.com/gnebehay/OpenTLD/downloads) that are available for Windows and Ubuntu 12.04.  
There is also a [PPA](https://launchpad.net/~opentld/+archive/ppa) available for Ubuntu 12.04. You just have to execute these commands:
```
sudo add-apt-repository ppa:opentld/ppa
sudo apt-get update
sudo apt-get install opentld
```

If you have a webcam attached to your PC, you can simply execute opentld (on Linux) or opentld.exe (on Windows) in order to
try it out. You can use the graphical configuration dialog as well, you just have to execite qopentld (on Linux) or qopentld.exe
(on Windows).  
For other configuration options, please see below.

A documentation of the internals as well as other possibly helpful information is contained in this [master thesis](https://github.com/downloads/gnebehay/OpenTLD/gnebehay_thesis_msc.pdf).

# Usage
## Keyboard shortcuts

* `q` quit
* `b` remember current frame as background model / clear background
* `c` clear model and stop tracking  git
* `l` toggle learning
* `a` toggle alternating mode (if true, detector is switched off when tracker is available)
* `e` export model to file specified in configuration parameter "modelExportFile"
* `i` import model from file specified in configuration parameter "modelPath"
* `r` clear model, let user reinit tracking

## Command line options
### Synopsis
`opentld [option arguments] [arguments]`

### option arguments
* `[-a <startFrameNumber>]` video starts at the frameNumber _startFrameNumber_
* `[-b <x,y,w,h>]` Initial bounding box
* `[-d <device>]` select input device: _device_=(IMGS|CAM|VID)  
	_IMGS_: capture from images  
	_CAM_: capture from connected camera  
	_VID_: capture from a video
* `[-e <path>]` export model after run to _path_
* `[-f]` shows foreground
* `[-i <path>]` _path_ to the images or to the video.
* `[-h]` shows help
* `[-m <path>]` if specified load a model from _path_. An initialBoundingBox must be specified or selectManually must be true.
* `[-n <number>]` Specifies the video device to use (defaults to 0). Useful to select a different camera when multiple cameras are connected.
* `[-p path]` prints results into the file _path_
* `[-s]` if set, user can select initial bounding box
* `[-t <theta>]` threshold for determining positive results
* `[-z <lastFrameNumber>]` video ends at the frameNumber _lastFrameNumber_.
	If _lastFrameNumber_ is 0 or the option argument isn't specified means
	all frames are taken.

### Arguments
`[CONFIG_FILE]` path to config file

## Config file
Look into the [sample-config-file](https://github.com/gnebehay/OpenTLD/blob/master/res/conf/config-sample.cfg) for more information.

# Building
## Dependencies
* OpenCV
* CMake
* libconfig++ (optional)
* Qt4 (optional)

## Compiling
### CMake
OpenTLD uses CMake to create native build environments such as make, Eclipse, Microsoft Visual Studio.
If you need help look at [Running CMake](http://www.cmake.org/cmake/help/runningcmake.html).

You can use `cmake-gui`, if you need a graphical user interface.

Use CMake to build the project. You can use "cmake-gui", if you need a graphical user interface.

__Gui__  
* Specify the source path (root path of the dictionary) and the binary path (where to build the program, out
	  of source build recommended)
* Configure
* Select compiler
* Adjust the options (if needed)
* Configure
* Generate

__Command line__  
If you have uncompressed the source in $OPENTLD, type in a console:
```bash
cd $OPENTLD
mkdir ../build
cd ../build
cmake ../$OPENTLD -DBUILD_QOPENTLD=ON -DUSE_SYSTEM_LIBS=OFF
```

__CMake options__  
* `BUILD_QOPENTLD` build the graphical configuration dialog (requieres Qt)
* `USE_SYSTEM_LIBS` don't use the included cvblob version but the installed version (requieres cvblob)

### Windows (Microsoft Visual Studio)
Navigate to the binary directory and build the solutions you want (You have to compile in RELEASE mode):
* `opentld` build the project
* `INSTALL` install the project

### Linux (make)
Navigate with the terminal to the build directory
* `make` build the project
* `make install` build and install the project

### Mac
* `brew install python`
* `brew install gfortran`
* `easy_install numpy`
* `brew install cmake`
* `brew install opencv`
* `make` build the project

# Debian package
* Navigate with the terminal into the root dictionary of OpenTLD (OpenTLD/)
* Type `debuild -us -uc`
* Delete the temporary files in the source tree with `debuild clean`

