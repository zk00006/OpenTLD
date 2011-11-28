add_library(tld
    Clustering.cpp
    DetectionResult.cpp
    DetectorCascade.cpp
    EnsembleClassifier.cpp
    ForegroundDetector.cpp
    MedianFlowTracker.cpp
    NNClassifier.cpp
    TLD.cpp
    TLDUtil.cpp
    VarianceFilter.cpp
    tracker/bb.cpp
    tracker/bb_predict.cpp
    tracker/fbtrack.cpp
    tracker/lk.cpp
    tracker/median.cpp)
    
#-------------------------------------------------------------------------------
#platform specific
#TODO: Make this configurable
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif(UNIX)

if(WINDOWS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
endif(WINDOWS)

