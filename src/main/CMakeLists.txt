#-------------------------------------------------------------------------------
#Compile
add_executable(main config.cpp export.cpp gui.cpp Main.cpp settings.cpp)

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
#link
target_link_libraries(main tld config cvblobs imacq ${OpenCV_LIBS})

if(WIN32)
target_link_libraries(main winmm) #Needed for time functions
endif(WIN32)

#-------------------------------------------------------------------------------
#install
install(TARGETS main DESTINATION bin)

if(NOT GENERATE_DEB_PACKAGE)
    install(FILES ${PROJECT_SOURCE_DIR}/conf/config-sample.cfg DESTINATION bin RENAME config.cfg)
endif(NOT GENERATE_DEB_PACKAGE)

set_target_properties(main PROPERTIES OUTPUT_NAME ${TLD_EXECUTABLE_NAME})