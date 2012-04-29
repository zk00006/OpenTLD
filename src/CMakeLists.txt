
#-------------------------------------------------------------------------------
#Compile
add_executable(opentld OpenTLD.cpp)

#-------------------------------------------------------------------------------
#link

target_link_libraries(opentld main)

set_target_properties(opentld PROPERTIES OUTPUT_NAME ../tld)

#-------------------------------------------------------------------------------
#install
install(TARGETS opentld DESTINATION bin)

