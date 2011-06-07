#Set minimum version requered
cmake_minimum_required(VERSION 2.4.6)
#just to avoid the warning
if(COMMAND cmake_policy)
     cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
#set project name
project(TLD)
#Append path to the module path
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
#OpenCV
find_package(OpenCV REQUIRED)
#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
#set the include directories
include_directories (../include	${OpenCV_INCLUDE_DIRS})
#libraries
add_library(tld_utils tld_utils.cpp)
add_library(LKTracker LKTracker.cpp)
#executables
add_executable(run_tld run_tld.cpp)
#link the libraries
target_link_libraries(run_tld LKTracker tld_utils ${OpenCV_LIBS})
