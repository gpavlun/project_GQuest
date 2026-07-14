file(GLOB DEVSTUDIO_SOURCES CONFIGURE_DEPENDS
    *.c
)

list(REMOVE_ITEM DEVSTUDIO_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/launcher-wrapper.c"
)

add_executable(devstudio
    ${DEVSTUDIO_SOURCES}
)

target_link_libraries(devstudio
    PRIVATE
        GQuest::logging
        GQuest::gcurses
        GQuest::gio
)

set_target_properties(devstudio PROPERTIES
    OUTPUT_NAME "devstudio.grg"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

add_executable(launcher-wrapper
    launcher-wrapper.c
)

set_target_properties(launcher-wrapper PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)