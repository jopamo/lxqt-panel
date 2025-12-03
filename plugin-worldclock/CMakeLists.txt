set(PLUGIN "worldclock")

set(HEADERS
    oneg4worldclock.h
    oneg4worldclockconfiguration.h
    oneg4worldclockconfigurationtimezones.h
    oneg4worldclockconfigurationmanualformat.h
)

set(SOURCES
    oneg4worldclock.cpp
    oneg4worldclockconfiguration.cpp
    oneg4worldclockconfigurationtimezones.cpp
    oneg4worldclockconfigurationmanualformat.cpp
)

set(UIS
    oneg4worldclockconfiguration.ui
    oneg4worldclockconfigurationtimezones.ui
    oneg4worldclockconfigurationmanualformat.ui
)

BUILD_ONEG4_PLUGIN(${PLUGIN})
