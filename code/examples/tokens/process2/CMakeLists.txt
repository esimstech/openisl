add_executable("process2" "")

target_include_directories("process2" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

target_link_directories("process2" PUBLIC ${Boost_LIBRARY_DIRS})

set(LIBS_TARGET "isl_api")
if(NOT MSVC)
    list(APPEND LIBS_TARGET "boost_program_options")
endif()

target_link_libraries("process2" ${LIBS_TARGET})

install(TARGETS "process2" CONFIGURATIONS Release DESTINATION "examples/tokens/process2/${PLATFORM_DIRECTORY}")

add_subdirectory("include")
add_subdirectory("src")
add_subdirectory("xml")
