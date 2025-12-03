# Common interface for Window Manager abstraction backend
# This also contains dummy backend

add_library(1g4-panel-backend-common STATIC

    oneg4taskbartypes.h
    ioneg4abstractwmiface.h
    ioneg4abstractwmiface.cpp

    oneg4dummywmbackend.h
    oneg4dummywmbackend.cpp
)

target_link_libraries(1g4-panel-backend-common
    Qt6::Gui
)

add_subdirectory(xcb)
