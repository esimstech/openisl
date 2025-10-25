add_library("isl_slavefmi" SHARED "")

target_include_directories("isl_slavefmi" PUBLIC
    "$ENV{FMI_PATH}"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

target_link_directories("isl_slavefmi" PUBLIC ${Boost_LIBRARY_DIRS})

set(LIBS_TARGET "isl_api")
if(NOT MSVC)
    list(APPEND LIBS_TARGET "boost_filesystem")
endif()

target_link_libraries("isl_slavefmi" ${LIBS_TARGET})

install(TARGETS "isl_slavefmi" RUNTIME DESTINATION "connect/fmi/lib/${PLATFORM_DIRECTORY}")

add_subdirectory("include")
add_subdirectory("src")
add_subdirectory("library")
