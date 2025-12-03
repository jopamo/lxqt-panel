MACRO (BUILD_ONEG4_PLUGIN NAME)
    set(PROGRAM "1g4-panel")
    project(${PROGRAM}_${NAME})

    set(PROG_SHARE_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/oneg4/${PROGRAM})
    set(PLUGIN_SHARE_DIR ${PROG_SHARE_DIR}/${NAME})

    # Desktop files **********************************
file (GLOB ${PROJECT_NAME}_DESKTOP_FILES_IN resources/*.desktop.in)
set(DESKTOP_FILES)
foreach(_desktop_in ${${PROJECT_NAME}_DESKTOP_FILES_IN})
        get_filename_component(_desktop_name ${_desktop_in} NAME_WE)
        set(_desktop "${CMAKE_CURRENT_BINARY_DIR}/${_desktop_name}")
        configure_file(${_desktop_in} ${_desktop} COPYONLY)
        list(APPEND DESKTOP_FILES ${_desktop})
endforeach()
    #************************************************

    file (GLOB CONFIG_FILES resources/*.conf)

    if (NOT DEFINED PLUGIN_DIR)
        set (PLUGIN_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/${PROGRAM})
    endif (NOT DEFINED PLUGIN_DIR)

    list(FIND STATIC_PLUGINS ${NAME} IS_STATIC)
    set(SRC
        ${HEADERS}
        ${SOURCES}
        ${MOC_SOURCES}
        ${RESOURCES}
        ${UIS}
        ${DESKTOP_FILES}
    )
    if (${IS_STATIC} EQUAL -1) # not static
        add_library(${NAME} MODULE ${SRC}) # build dynamically loadable modules
        install(TARGETS ${NAME} DESTINATION ${PLUGIN_DIR}) # install the *.so file
    else() # static
        add_library(${NAME} STATIC ${SRC}) # build statically linked lib
    endif()
    target_link_libraries(${NAME}
        Qt6::Widgets
        oneg4-compat
        ${LIBRARIES}
        KF6::WindowSystem
    )

    install(FILES ${CONFIG_FILES}  DESTINATION ${PLUGIN_SHARE_DIR})
    install(FILES ${DESKTOP_FILES} DESTINATION ${PROG_SHARE_DIR})

ENDMACRO(BUILD_ONEG4_PLUGIN)
