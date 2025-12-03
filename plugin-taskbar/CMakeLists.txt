set(PLUGIN "taskbar")

set(HEADERS
    oneg4taskbar.h
    oneg4taskbutton.h
    oneg4taskbarconfiguration.h
    oneg4taskbarplugin.h
    oneg4taskgroup.h
    oneg4grouppopup.h
)

set(SOURCES
    oneg4taskbar.cpp
    oneg4taskbutton.cpp
    oneg4taskbarconfiguration.cpp
    oneg4taskbarplugin.cpp
    oneg4taskgroup.cpp
    oneg4grouppopup.cpp
)

set(UIS
    oneg4taskbarconfiguration.ui
)

set(LIBRARIES)

BUILD_ONEG4_PLUGIN(${PLUGIN})
