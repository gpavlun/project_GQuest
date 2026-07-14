add_executable(editor)

file(GLOB EDITOR_SOURCES CONFIGURE_DEPENDS
    *.c
)

target_sources(editor
    PRIVATE
        ${EDITOR_SOURCES}
)

target_link_libraries(editor
    PRIVATE
        PkgConfig::SDL2
        GQuest::logging
        GQuest::gcurses
        GQuest::gio
)

set_target_properties(editor PROPERTIES
    OUTPUT_NAME "editor.grg"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)