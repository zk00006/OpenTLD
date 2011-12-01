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


project(tld)

cmake_minimum_required(VERSION 2.6)

if(NOT GENERATE_DEB_PACKAGE)
	FIND_PACKAGE( OpenCV REQUIRED )
endif(NOT GENERATE_DEB_PACKAGE)

if(GENERATE_DEB_PACKAGE)
	include_directories(/usr/include/opencv)
endif(GENERATE_DEB_PACKAGE)

#-------------------------------------------------------------------------------
#version
set(TLD_VERSION_MAJOR 1)
set(TLD_VERSION_MINOR 1)
set(TLD_VERSION_PATCH 0)
set(TLD_VERSION ${TLD_VERSION_MAJOR}.${TLD_VERSION_MINOR}.${TLD_VERSION_PATCH})

#-------------------------------------------------------------------------------
#executable name
set(TLD_EXECUTABLE_NAME tld)

#------------------------------------------------------------------------------
#options
option(BUILD_WITH_QT "Build the program with QT-GUI support." OFF)

if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
    option(GENERATE_DEB_PACKAGE "Gererates a Debian (.deb) package." OFF)
endif(${CMAKE_SYSTEM_NAME} MATCHES Linux)

#------------------------------------------------------------------------------
#CPack
if(GENERATE_DEB_PACKAGE)
    set(CPACK_GENERATOR DEB)
endif(GENERATE_DEB_PACKAGE)
set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR ${TLD_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${TLD_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${TLD_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${TLD_VERSION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Georg Nebehay")
if(BUILD_WITH_QT)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtgui4 (>= 4.0.0), libqtcore4 (>= 4.0.0), libhighgui4 (>= 2.0.0)")
endif(BUILD_WITH_QT)
include(CPack)

#------------------------------------------------------------------------------
#find packages
if(BUILD_WITH_QT)
    find_package(Qt4)
endif(BUILD_WITH_QT)

#------------------------------------------------------------------------------
#package specific
if(BUILD_WITH_QT)
    if(QT4_FOUND)
        include(${QT_USE_FILE})
        add_definitions(-DWITH_QT)
    else(QT4_FOUND)
        message(FATAL_ERROR "Unable to find Qt4!")      
    endif(QT4_FOUND)
endif(BUILD_WITH_QT)

#------------------------------------------------------------------------------
#build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

#-------------------------------------------------------------------------------
#compiler and linker options
include_directories(${PROJECT_SOURCE_DIR}/src/cvblobs ${PROJECT_SOURCE_DIR}/src/config ${PROJECT_SOURCE_DIR}/src/tld ${PROJECT_SOURCE_DIR}/src/tld/tracker ${PROJECT_SOURCE_DIR}/src/imacq ${PROJECT_SOURCE_DIR}/src)
#link_directories(${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if(WIN32)
	add_definitions(-DLIBCONFIGXX_STATIC -DLIBCONFIG_STATIC) #Needed when linking libconfig statically
endif(WIN32)

#-------------------------------------------------------------------------------
#add subdirectories
add_subdirectory(src/imacq)
add_subdirectory(src/cvblobs)
add_subdirectory(src/config)
add_subdirectory(src/tld)
add_subdirectory(src/main)



#-------------------------------------------------------------------------------
#configure & install
if(NOT GENERATE_DEB_PACKAGE)
    install(FILES README DESTINATION .)
else(NOT GENERATE_DEB_PACKAGE)
    configure_file(tld.desktop.in
        ${PROJECT_BINARY_DIR}/tld.desktop)
    install(FILES ${PROJECT_BINARY_DIR}/tld.desktop DESTINATION share/applications)
    install(FILES res/tld.png DESTINATION share/app-install/icons)
endif(NOT GENERATE_DEB_PACKAGE)

#-------------------------------------------------------------------------------
#messages
message(STATUS)
message(STATUS "-------------------------------------------------------------------------------")
message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message(STATUS "Change a value with: cmake -D<Variable>=<Value>")
message(STATUS "-------------------------------------------------------------------------------")
message(STATUS)

