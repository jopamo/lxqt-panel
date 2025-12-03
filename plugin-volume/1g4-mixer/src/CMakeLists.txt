# include_directories replaced by target_include_directories

set(1g4-mixer_HDRS
    1g4-mixer.h
    mainwindow.h
    cardwidget.h
    channel.h
    devicewidget.h
    minimalstreamwidget.h
    rolewidget.h
    sinkinputwidget.h
    sinkwidget.h
    sourceoutputwidget.h
    sourcewidget.h
    streamwidget.h
    elidinglabel.h
)

set(1g4-mixer_SRCS
    1g4-mixer.cc
    mainwindow.cc
    cardwidget.cc
    channel.cc
    devicewidget.cc
    minimalstreamwidget.cc
    rolewidget.cc
    sinkinputwidget.cc
    sinkwidget.cc
    sourceoutputwidget.cc
    sourcewidget.cc
    streamwidget.cc
    elidinglabel.cc
)

set(1g4-mixer_UI
    cardwidget.ui
    devicewidget.ui
    mainwindow.ui
    streamwidget.ui
)

add_library(1g4-mixer STATIC
    ${1g4-mixer_SRCS}
)
target_compile_definitions(1g4-mixer PRIVATE ONEG4_MIXER_LIBRARY=1)
target_include_directories(1g4-mixer SYSTEM PUBLIC ${PULSE_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
set_property(
     TARGET 1g4-mixer APPEND
     PROPERTY COMPILE_DEFINITIONS
     ONEG4_MIXER_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}"
)

target_link_libraries(1g4-mixer
    Qt6::Widgets
    ${PULSE_LDFLAGS}
    ${GLIB_LDFLAGS}
)

