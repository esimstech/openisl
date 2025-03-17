set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/install.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/package.mo"
)

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "connect/openmodelica/OpenISL")
endif()