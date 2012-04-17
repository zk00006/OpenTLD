cmake_minimum_required(VERSION 2.6)

find_package(OpenCV REQUIRED)

add_library(cvblobs blob.cpp BlobContour.cpp BlobOperators.cpp BlobProperties.cpp BlobResult.cpp ComponentLabeling.cpp)
