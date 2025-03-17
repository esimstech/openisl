add_library("isl_utils" STATIC "")

target_include_directories("isl_utils" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

add_subdirectory("include")
add_subdirectory("src")