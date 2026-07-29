set(LIBRARY_NAME unity)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.*"
)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC include)
target_include_directories(${LIBRARY_NAME} PUBLIC src)
