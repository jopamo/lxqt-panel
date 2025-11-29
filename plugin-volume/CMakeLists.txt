set(PLUGIN "volume")

set(HEADERS
    lxqtvolume.h
    volumebutton.h
    volumepopup.h
    audiodevice.h
    lxqtvolumeconfiguration.h
    audioengine.h
)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/panel/lxqtpanelpluginconfigdialog.cpp
    lxqtvolume.cpp
    volumebutton.cpp
    volumepopup.cpp
    audiodevice.cpp
    lxqtvolumeconfiguration.cpp
    audioengine.cpp
    pulseaudioengine.cpp
)

set(UIS
    lxqtvolumeconfiguration.ui
)

set(LIBRARIES
    ${LIBRARIES}
    Qt6Xdg
)

add_definitions(-DUSE_PULSEAUDIO)
include_directories(${PULSEAUDIO_INCLUDE_DIR})
set(HEADERS ${HEADERS} pulseaudioengine.h)
set(SOURCES ${SOURCES} pulseaudioengine.cpp)
set(MOCS ${MOCS} pulseaudioengine.h)
set(LIBRARIES ${LIBRARIES} ${PULSEAUDIO_LIBRARY})

BUILD_LXQT_PLUGIN(${PLUGIN})
