set(PLUGIN "showdesktop")

set(HEADERS
    showdesktop.h
)

set(SOURCES
    showdesktop.cpp
)

set(LIBRARIES
    ${LIBRARIES}
    Qt6Xdg
)

BUILD_LXQT_PLUGIN(${PLUGIN})
