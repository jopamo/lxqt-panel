set(PLUGIN "statusnotifier")



find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Concurrent)

set(HEADERS
    statusnotifier.h
    statusnotifierconfiguration.h
    dbustypes.h
    statusnotifierbutton.h
    statusnotifieriteminterface.h
    statusnotifierwatcher.h
    statusnotifierwidget.h
    sniasync.h
    statusnotifierproxy.h
)

set(SOURCES
    statusnotifier.cpp
    statusnotifierconfiguration.cpp
    dbustypes.cpp
    statusnotifierbutton.cpp
    statusnotifieriteminterface.cpp
    statusnotifierwatcher.cpp
    statusnotifierwidget.cpp
    sniasync.cpp
    statusnotifierproxy.cpp
)

set(UIS
    statusnotifierconfiguration.ui
)

qt6_add_dbus_adaptor(DBUS_SOURCES
    org.kde.StatusNotifierItem.xml
    statusnotifieriteminterface.h
    StatusNotifierItemInterface
)

set_source_files_properties(${DBUS_SOURCES} PROPERTIES SKIP_AUTOGEN ON)

list(APPEND SOURCES "${DBUS_SOURCES}")

set(LIBRARIES
    Qt6::Concurrent
    Qt6::DBus
)

BUILD_ONEG4_PLUGIN(${PLUGIN})
