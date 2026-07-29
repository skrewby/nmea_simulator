set(LIBRARY_NAME nmea2000)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.*"
)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()

add_library(${LIBRARY_NAME} STATIC ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC include)
target_include_directories(${LIBRARY_NAME} PRIVATE src)
target_link_libraries(${LIBRARY_NAME} PUBLIC m)
target_link_libraries(${LIBRARY_NAME} PRIVATE warnings)

if(ENABLE_TESTS)
    target_link_libraries(${LIBRARY_NAME} PRIVATE sanitizers)
endif()
