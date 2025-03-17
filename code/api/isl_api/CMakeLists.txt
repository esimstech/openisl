add_library("isl_api" SHARED "")

target_include_directories("isl_api" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

target_link_directories("isl_api" PUBLIC ${Boost_LIBRARY_DIRS})

set(LIBS_TARGET "isl_utils")
if(NOT MSVC)
    list(APPEND LIBS_TARGET "boost_filesystem" "boost_thread" "boost_chrono")
endif()

target_link_libraries("isl_api" ${LIBS_TARGET})

install(TARGETS "isl_api" CONFIGURATIONS Release DESTINATION "bin/${PLATFORM_DIRECTORY}")

if(MSVC)
    add_compile_definitions(ISL_API_LIB)
endif()

add_subdirectory("include")
add_subdirectory("src")
add_subdirectory("resources")
