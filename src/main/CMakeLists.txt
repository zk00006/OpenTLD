set(MAIN_SOURCES config.cpp export.cpp gui.cpp Main.cpp settings.cpp qtconfiggui.cpp)

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

#-------------------------------------------------------------------------------
#Compile
add_library(main ${MAIN_SOURCES})
