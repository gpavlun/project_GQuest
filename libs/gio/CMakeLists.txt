add_library(gio STATIC)

file(GLOB GIO_SOURCES CONFIGURE_DEPENDS
    src/*.c
)

target_sources(gio
    PRIVATE
        ${GIO_SOURCES}
)

target_include_directories(gio
    PUBLIC
        include
)

add_library(GQuest::gio ALIAS gio)