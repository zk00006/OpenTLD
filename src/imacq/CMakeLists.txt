cmake_minimum_required(VERSION 2.6)

find_package(OpenCV REQUIRED)

add_library(imacq imAcq.cpp)

target_link_libraries(imacq ${OpenCV_LIBS})

