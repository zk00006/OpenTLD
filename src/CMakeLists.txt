
#-------------------------------------------------------------------------------
#Compile
add_executable(opentld OpenTLD.cpp)

#-------------------------------------------------------------------------------
#link

target_link_libraries(opentld main tld config cvblobs imacq)

if(GENERATE_DEB_PACKAGE)
	target_link_libraries(opentld highgui)
endif(GENERATE_DEB_PACKAGE)

if(NOT GENERATE_DEB_PACKAGE)
	target_link_libraries(opentld ${OpenCV_LIBS})
endif(NOT GENERATE_DEB_PACKAGE)


if(BUILD_WITH_QT)
    target_link_libraries(opentld ${QT_LIBRARIES})
endif(BUILD_WITH_QT)


if(WIN32)
target_link_libraries(opentld winmm) #Needed for time functions
endif(WIN32)

#-------------------------------------------------------------------------------
#install
install(TARGETS opentld DESTINATION bin)

if(NOT GENERATE_DEB_PACKAGE)
    install(FILES ${PROJECT_SOURCE_DIR}/conf/config-sample.cfg DESTINATION bin RENAME config.cfg)
endif(NOT GENERATE_DEB_PACKAGE)

#-------------------------------------------------------------------------------
#platform specific
#TODO: Make this configurable
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif(UNIX)

if(WINDOWS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
endif(WINDOWS)

#-------------------------------------------------------------------------------
#Qt4 specific
if(BUILD_WITH_QT)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    set(TLD_QT_HEADERS qtconfiggui.h)
    set(TLD_QT_FORMS qtconfiggui.ui)
    QT4_WRAP_CPP(TLD_QT_HEADERS_MOC ${TLD_QT_HEADERS})
    QT4_WRAP_UI(TLD_QT_FORMS_HEADERS ${TLD_QT_FORMS})
    set(MAIN_SOURCES ${MAIN_SOURCES} ${TLD_QT_HEADERS_MOC} ${TLD_QT_FORMS_HEADERS})
endif(BUILD_WITH_QT)


set_target_properties(opentld PROPERTIES OUTPUT_NAME ${TLD_EXECUTABLE_NAME})