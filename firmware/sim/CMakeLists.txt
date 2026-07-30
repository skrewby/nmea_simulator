set(LIBRARY_NAME bsp_sim)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.*"
)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/bsp/include)
target_include_directories(${LIBRARY_NAME} PRIVATE src)
target_link_libraries(${LIBRARY_NAME} PRIVATE warnings sanitizers civetweb jansson)
target_compile_definitions(${LIBRARY_NAME} PRIVATE
    FIRMWARE_DEVICE_NAME="${PROJECT_NAME}"
    FIRMWARE_VERSION="${PROJECT_VERSION}"
    FIRMWARE_WEB_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/web"
)
