set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/install.txt"
)

if(WIN32)
    list(APPEND PUBLIC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/00_create_venv.bat"
        "${CMAKE_CURRENT_SOURCE_DIR}/00_env.bat"
        "${CMAKE_CURRENT_SOURCE_DIR}/01_install.bat"
        "${CMAKE_CURRENT_SOURCE_DIR}/01_start.bat"
    )
else()
    list(APPEND PUBLIC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/00_create_venv.sh"
        "${CMAKE_CURRENT_SOURCE_DIR}/00_env.sh"
        "${CMAKE_CURRENT_SOURCE_DIR}/01_install.sh"
        "${CMAKE_CURRENT_SOURCE_DIR}/01_start.sh"
    )
endif()

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "connect/python")
endif()