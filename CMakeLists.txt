# Open The CMake GUI
# specify the source directory and the binary directory
# press configure
# set CMAKE_INSTALL_PREFIX to the path where you want to install the program
# press configure
# check BUILD_WITH_QT if you want to build the program with a QT-Config GUI
# check GENERATE_DEB_PACKAGE if you want to build a debian package (only on Linux)
#
# UNIX Makefile:
# 1) go to the binary folder and type "make" to build the project
# 2) (optional) type "make install all" to install the files into the install
#    directory
# 3) (optional) type "make package" to create a package of the install folder
#    (.tgz file if GENERATE_DEB_PACKAGE=false, .deb file if GENERATE_DEB_PACKAGE=true)
#
# Microsoft Visual C++:
# 1) open the .sln file
# 2) change the mode to "Release" -> only Release is supported!
# 3) build the project "ALL_BUILD" to build the tld project
# 4) build the project "INSTALL" to install the files into the install
#    directory
# 5) build the project "PACKAGE" to create an NSIS-installer (NSIS is required)


project(OpenTLD)

cmake_minimum_required(VERSION 2.6)

#-------------------------------------------------------------------------------
#version
set(TLD_VERSION_MAJOR 1)
set(TLD_VERSION_MINOR 2)
set(TLD_VERSION_PATCH 0)
set(TLD_VERSION ${TLD_VERSION_MAJOR}.${TLD_VERSION_MINOR}.${TLD_VERSION_PATCH})

#------------------------------------------------------------------------------
#build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(WIN32)
	add_definitions(-DLIBCONFIGXX_STATIC -DLIBCONFIG_STATIC) #Needed when linking libconfig statically
endif(WIN32)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgomp")

include_directories(src/cvblobs src/imacq src/config src/tld src/mftracker src/main)

#-------------------------------------------------------------------------------
#add subdirectories
add_subdirectory(src/imacq)
add_subdirectory(src/cvblobs)
add_subdirectory(src/config)
add_subdirectory(src/tld)
add_subdirectory(src/mftracker)
add_subdirectory(src/main)
add_subdirectory(src)

configure_file("${PROJECT_SOURCE_DIR}/OpenTLDConfig.cmake.in" "${PROJECT_BINARY_DIR}/OpenTLDConfig.cmake" @ONLY)

