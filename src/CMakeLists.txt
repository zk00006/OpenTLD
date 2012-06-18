#-------------------------------------------------------------------------------
#Compile
add_executable(opentld OpenTLD.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})

#-------------------------------------------------------------------------------
#link

link_directories(${OpenCV_LIB_DIR})

target_link_libraries(opentld config cvblobs imacq main mftracker tld ${OpenCV_LIBS})

#set_target_properties(opentld PROPERTIES OUTPUT_NAME ../opentld)

#-------------------------------------------------------------------------------
#install
install(TARGETS opentld DESTINATION bin)

