set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/install.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml"
)

if(WIN32)
    list(APPEND PUBLIC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/buildislfmu.bat"
        "${CMAKE_CURRENT_SOURCE_DIR}/islfmuenv.bat"
    )
else()
    list(APPEND PUBLIC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/buildislfmu.sh"
        "${CMAKE_CURRENT_SOURCE_DIR}/islfmuenv.sh"
    )
endif()

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "connect/fmi")
endif()