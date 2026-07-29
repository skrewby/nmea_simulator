set(LIBRARY_NAME bsp)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.*"
)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC include)
target_include_directories(${LIBRARY_NAME} PRIVATE src)
target_link_libraries(${LIBRARY_NAME} PRIVATE hal warnings)
target_link_options(${LIBRARY_NAME} INTERFACE
    "SHELL:-Wl,--whole-archive $<TARGET_FILE:hal> -Wl,--no-whole-archive"
)
