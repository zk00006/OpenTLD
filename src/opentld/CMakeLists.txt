
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
    main/Config.cpp
    main/Gui.cpp
    main/Main.cpp
    main/Settings.cpp
    main/Config.h
    main/Gui.h
    main/Main.h
    main/Settings.h)

target_link_libraries(opentld libopentld cvblobs libconfig ${OpenCV_LIBS})

install(TARGETS opentld DESTINATION bin)

