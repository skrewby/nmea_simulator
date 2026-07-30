if(ENABLE_TESTS)
    add_subdirectory(unity)
endif()

if(ENABLE_SIMULATOR)
    add_subdirectory(civetweb)

    set(JANSSON_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(JANSSON_INSTALL OFF CACHE BOOL "" FORCE)
    set(JANSSON_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(JANSSON_WITHOUT_TESTS ON CACHE BOOL "" FORCE)
    add_subdirectory(jansson)
endif()
