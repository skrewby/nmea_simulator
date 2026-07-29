cmake_minimum_required(VERSION 4.0)
project(nmea_sim VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if(NOT PRESET_NAME)
    set(PRESET_NAME "default")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PRESET_NAME})

option(ENABLE_TESTS "Enable building and running unit tests" OFF)

if(ENABLE_TESTS)
    enable_testing()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

add_library(warnings INTERFACE)
target_compile_options(warnings INTERFACE
    -Wall
    -Wextra
    -Wshadow
    -Wdouble-promotion
)

if(ENABLE_TESTS)
    add_library(sanitizers INTERFACE)
    target_compile_options(sanitizers INTERFACE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -g
    )
    target_link_options(sanitizers INTERFACE
        -fsanitize=address,undefined
    )
endif()

add_subdirectory(vendor)
add_subdirectory(lib)

if(NOT ENABLE_TESTS)
    add_subdirectory(hal)
    add_subdirectory(bsp)

    file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "app/*.*"
    )

    configure_file(cmake/version.h.in ${CMAKE_BINARY_DIR}/generated/version.h)

    add_executable(${PROJECT_NAME} ${SOURCES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/generated)
    target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:FW_ASSERT_DEBUG>)

    target_link_libraries(${PROJECT_NAME} PRIVATE bsp nmea2000 warnings)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.hex
        COMMENT "Generating ${PROJECT_NAME}.bin and ${PROJECT_NAME}.hex"
    )
endif()
