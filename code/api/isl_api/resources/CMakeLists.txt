set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl.ico"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_api.ini"
)

if(NOT MSVC)
    list(APPEND PUBLIC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/kill_ipcs.sh")
endif()

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "resources")
endif()