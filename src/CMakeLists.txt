
#-------------------------------------------------------------------------------
#sources
set(TLD_SOURCES
    config.cpp
    export.cpp
    gui.cpp
    Main.cpp
    settings.cpp
    imacq/imAcq.cpp
    tld/Clustering.cpp
    tld/DetectionResult.cpp
    tld/DetectorCascade.cpp
    tld/EnsembleClassifier.cpp
    tld/ForegroundDetector.cpp
    tld/MedianFlowTracker.cpp
    tld/NNClassifier.cpp
    tld/TLD.cpp
    tld/TLDUtil.cpp
    tld/VarianceFilter.cpp
    tld/tracker/bb.cpp
    tld/tracker/bb_predict.cpp
    tld/tracker/fbtrack.cpp
    tld/tracker/lk.cpp
    tld/tracker/median.cpp)

#-------------------------------------------------------------------------------
#Qt4 specific
if(BUILD_WITH_QT)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    set(TLD_QT_HEADERS ui/qtconfiggui.h)
    set(TLD_QT_FORMS ui/qtconfiggui.ui)
    QT4_WRAP_CPP(TLD_QT_HEADERS_MOC ${TLD_QT_HEADERS})
    QT4_WRAP_UI(TLD_QT_FORMS_HEADERS ${TLD_QT_FORMS})
    set(TLD_LINK_LIBS ${TLD_LINK_LIBS} ${QT_LIBRARIES})
    set(TLD_SOURCES ${TLD_SOURCES} ${TLD_QT_HEADERS_MOC} ${TLD_QT_FORMS_HEADERS})
endif(BUILD_WITH_QT)

#-------------------------------------------------------------------------------
#build
add_library(cvblobs cvblobs/blob.cpp cvblobs/BlobContour.cpp cvblobs/BlobOperators.cpp cvblobs/BlobProperties.cpp cvblobs/BlobResult.cpp cvblobs/ComponentLabeling.cpp)
add_library(config config/grammar.c config/libconfig.c config/libconfigcpp.cc config/scanctx.c config/scanner.c config/strbuf.c)
add_executable(tld ${TLD_SOURCES})
set_target_properties(tld PROPERTIES OUTPUT_NAME ${TLD_EXECUTABLE_NAME})

#-------------------------------------------------------------------------------
#link
target_link_libraries(tld config cvblobs ${OpenCV_LIBS})

if(WIN32)
target_link_libraries(tld winmm)
	add_definitions(-DLIBCONFIGXX_STATIC -DLIBCONFIG_STATIC -DYY_NO_UNISTD_H -DYY_USE_CONST -D_CRT_SECURE_NO_DEPRECATE -D_STDLIB_H)
endif(WIN32)

#-------------------------------------------------------------------------------
#install
install(TARGETS tld DESTINATION bin)

if(NOT GENERATE_DEB_PACKAGE)
    install(FILES ${PROJECT_SOURCE_DIR}/conf/config-sample.cfg DESTINATION bin RENAME config.cfg)
endif(NOT GENERATE_DEB_PACKAGE)