cmake_minimum_required(VERSION 3.20)
project(nmea_simulator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_SOURCES
    src/main.cpp
    src/config.cpp
)
set(PROJECT_INCLUDE_PATHS
    src
    lib
)

add_subdirectory(lib)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
target_link_libraries(${PROJECT_NAME} PRIVATE nmea)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE_PATHS})
