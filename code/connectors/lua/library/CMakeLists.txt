set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/install.txt"
)

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "connect/lua")
endif()