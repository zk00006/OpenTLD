
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
#libraries to link
if(UNIX)
    set(TLD_LINK_LIBS highgui config++ cvblobs)
endif(UNIX)

if(WIN32)
    set(TLD_LINK_LIBS cv200 cvblobslib cxcore200 highgui200 libconfig++ winmm)
endif(WIN32)

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
add_executable(tld ${TLD_SOURCES})
set_target_properties(tld PROPERTIES OUTPUT_NAME ${TLD_EXECUTABLE_NAME})

#-------------------------------------------------------------------------------
#link
target_link_libraries(tld ${TLD_LINK_LIBS})

#-------------------------------------------------------------------------------
#install
install(TARGETS tld DESTINATION bin)

if(NOT GENERATE_DEB_PACKAGE)
    install(FILES ${PROJECT_SOURCE_DIR}/conf/config-sample.cfg DESTINATION bin RENAME config.cfg)
endif(NOT GENERATE_DEB_PACKAGE)


if(WIN32)
    install(FILES
            ${PROJECT_SOURCE_DIR}/dll/cv200.dll
            ${PROJECT_SOURCE_DIR}/dll/cxcore200.dll
            ${PROJECT_SOURCE_DIR}/dll/highgui200.dll
            ${PROJECT_SOURCE_DIR}/dll/libconfig++.dll
        DESTINATION bin)
    if(BUILD_WITH_QT)
		install(FILES
		        ${QT_BINARY_DIR}/QtCore4.dll
		        ${QT_BINARY_DIR}/QtGui4.dll
		    DESTINATION bin)
	endif(BUILD_WITH_QT)
endif(WIN32)
