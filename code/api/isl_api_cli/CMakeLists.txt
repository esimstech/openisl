add_library("isl_cli" SHARED "")

target_include_directories("isl_cli" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

target_link_libraries("isl_cli" "isl_api")

set_target_properties("isl_cli" PROPERTIES COMMON_LANGUAGE_RUNTIME "")

install(TARGETS "isl_cli" CONFIGURATIONS Release DESTINATION "bin/${PLATFORM_DIRECTORY}")

add_subdirectory("include")
add_subdirectory("src")
add_subdirectory("resources")
