add_library(objects STATIC)

file(GLOB OBJECTS_SOURCES CONFIGURE_DEPENDS
    src/*.c
)

target_sources(objects
    PRIVATE
        ${OBJECTS_SOURCES}
)

target_include_directories(objects
    PUBLIC
        include
)

add_library(GQuest::objects ALIAS objects)