
link_directories(${OpenCV_LIB_DIR})

include_directories(main
    ../libopentld/imacq
    ../libopentld/mftracker
    ../libopentld/tld
    ../3rdparty/cvblobs
    ../3rdparty/libconfig
    ${OpenCV_INCLUDE_DIRS})

add_executable(opentld
    OpenTLD.cpp
    main/config.cpp
    main/gui.cpp
    main/Main.cpp
    main/settings.cpp)

target_link_libraries(opentld libopentld cvblobs libconfig ${OpenCV_LIBS})

install(TARGETS opentld DESTINATION bin)

