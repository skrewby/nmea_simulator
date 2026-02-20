include(FetchContent)
FetchContent_Declare(
    nmea
    GIT_REPOSITORY https://github.com/skrewby/nmea
    GIT_TAG        master
)
set(NMEA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(NMEA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nmea)

