set(PROJECT 1g4-panel)

# Window Manager backends
add_subdirectory(backends)

set(PRIV_HEADERS
    panelpluginsmodel.h
    windownotifier.h
    oneg4panel.h
    oneg4panelapplication.h
    oneg4panelapplication_p.h
    oneg4panellayout.h
    plugin.h
    pluginsettings_p.h
    oneg4panellimits.h
    popupmenu.h
    pluginmoveprocessor.h
    oneg4panelpluginconfigdialog.h
    config/configpaneldialog.h
    config/configplacement.h
    config/configstyling.h
    config/configpluginswidget.h
    config/addplugindialog.h
)

# using OneG4 namespace in the public headers.
set(PUB_HEADERS
    oneg4panelglobals.h
    pluginsettings.h
    ioneg4panelplugin.h
    ioneg4panel.h
)

set(SOURCES
    main.cpp
    panelpluginsmodel.cpp
    windownotifier.cpp
    oneg4panel.cpp
    oneg4panelapplication.cpp
    oneg4panellayout.cpp
    plugin.cpp
    pluginsettings.cpp
    popupmenu.cpp
    pluginmoveprocessor.cpp
    oneg4panelpluginconfigdialog.cpp
    config/configpaneldialog.cpp
    config/configplacement.cpp
    config/configstyling.cpp
    config/configpluginswidget.cpp
    config/addplugindialog.cpp
)

set(UI
    config/configplacement.ui
    config/configstyling.ui
    config/configpluginswidget.ui
    config/addplugindialog.ui
)

set(LIBRARIES
    oneg4-compat
)

file(GLOB CONFIG_FILES resources/*.conf)

############################################

add_definitions(-DCOMPILE_ONEG4_PANEL)

set(PLUGIN_DESKTOPS_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/oneg4/${PROJECT}")
add_definitions(-DPLUGIN_DESKTOPS_DIR=\"${PLUGIN_DESKTOPS_DIR}\")

if (WITH_SCREENSAVER_FALLBACK)
    message(STATUS "Building with conversion of deprecated 'screensaver' plugin")
    add_definitions(-DWITH_SCREENSAVER_FALLBACK "-DONEG4_LOCK_DESKTOP=\"${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications/oneg4-lockscreen.desktop\"")
endif ()

project(${PROJECT})

set(QTX_LIBRARIES Qt6::Widgets Qt6::Xml Qt6::DBus)


add_executable(${PROJECT}
    ${PUB_HEADERS}
    ${PRIV_HEADERS}
    ${SOURCES}
    ${UI}
)

target_link_libraries(${PROJECT}
    ${LIBRARIES}
    ${QTX_LIBRARIES}
    KF6::WindowSystem
    ${STATIC_PLUGINS}
    1g4-panel-backend-common
)

set_property(TARGET ${PROJECT} PROPERTY ENABLE_EXPORTS TRUE)

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES ${CONFIG_FILES} DESTINATION "${ONEG4_ETC_XDG_DIR}/oneg4")
install(FILES ${PUB_HEADERS} DESTINATION include/oneg4)
install(FILES
    man/1g4-panel.1
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
    COMPONENT Runtime
)
