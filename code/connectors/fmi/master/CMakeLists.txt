add_executable("islmasterfmi" "")

target_include_directories("islmasterfmi" PUBLIC
    "$ENV{FMI_PATH}"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

target_link_directories("islmasterfmi" PUBLIC ${Boost_LIBRARY_DIRS})

set(LIBS_TARGET "isl_api")
if(NOT MSVC)
    list(APPEND LIBS_TARGET "boost_program_options")
endif()

target_link_libraries("islmasterfmi" ${LIBS_TARGET})

install(TARGETS "islmasterfmi" CONFIGURATIONS Release DESTINATION "bin/${PLATFORM_DIRECTORY}")

add_subdirectory("include")
add_subdirectory("src")
