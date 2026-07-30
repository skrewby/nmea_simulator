set(LIBRARY_NAME civetweb)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.*"
)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC include)
target_include_directories(${LIBRARY_NAME} PRIVATE src)
target_compile_definitions(${LIBRARY_NAME} PRIVATE
    NO_SSL
    NO_CGI
    USE_WEBSOCKET
)

find_package(Threads REQUIRED)
target_link_libraries(${LIBRARY_NAME} PUBLIC Threads::Threads)
