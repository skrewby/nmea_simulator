cmake_minimum_required(VERSION 4.0)

if(ENABLE_TESTS)
    project(lib_tests C)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS ON)

    enable_testing()
endif()

add_subdirectory(nmea2000)
