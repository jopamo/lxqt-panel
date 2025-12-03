set(AUTOSTART_DESKTOP_FILES_IN 1g4-panel.desktop.in)

# Desktop file
configure_file(${AUTOSTART_DESKTOP_FILES_IN} ${CMAKE_CURRENT_BINARY_DIR}/1g4-panel.desktop COPYONLY)
set(DESKTOP_FILES ${CMAKE_CURRENT_BINARY_DIR}/1g4-panel.desktop)
add_custom_target(oneg4_panel_autostart_desktop_files ALL DEPENDS ${DESKTOP_FILES})

install(FILES
    ${DESKTOP_FILES}
    DESTINATION "${ONEG4_ETC_XDG_DIR}/autostart"
    COMPONENT Runtime
)

