add_library(mftracker
    bb.cpp
    bb_predict.cpp
    fbtrack.cpp
    lk.cpp
    median.cpp)

target_link_libraries(mftracker ${OpenCV_LIBS})
