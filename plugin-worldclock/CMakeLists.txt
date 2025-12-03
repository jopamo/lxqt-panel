set(PLUGIN "worldclock")

set(HEADERS
    oneg4worldclock.h
    oneg4worldclockconfiguration.h
    oneg4worldclockconfigurationtimezones.h
    worldclockmanualformatconfig.h
)

set(SOURCES
    oneg4worldclock.cpp
    oneg4worldclockconfiguration.cpp
    oneg4worldclockconfigurationtimezones.cpp
    worldclockmanualformatconfig.cpp
)

set(UIS
    oneg4worldclockconfiguration.ui
    oneg4worldclockconfigurationtimezones.ui
    worldclockmanualformatconfig.ui
)

BUILD_ONEG4_PLUGIN(${PLUGIN})
