add_library(logging STATIC)

file(GLOB LOGGING_SOURCES CONFIGURE_DEPENDS
    src/*.c
)

target_sources(logging
    PRIVATE
        ${LOGGING_SOURCES}
)

target_include_directories(logging
    PUBLIC
        include
)

target_link_libraries(logging
    PUBLIC
        GQuest::objects
)

add_library(GQuest::logging ALIAS logging)