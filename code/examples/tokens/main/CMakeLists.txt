set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt"
)

if(MSVC)
    if("${PLATFORM_DIRECTORY}" STREQUAL "x64")
        list(APPEND PUBLIC_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/run_x64.bat"
        )
    else()
        list(APPEND PUBLIC_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/run_x86.bat"
        )
    endif()
else()
    if("${PLATFORM_DIRECTORY}" STREQUAL "x64")
        list(APPEND PUBLIC_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/run_x64.sh"
        )
    else()
        list(APPEND PUBLIC_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/run_x86.sh"
        )
    endif()
endif()

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "examples/tokens")
endif()