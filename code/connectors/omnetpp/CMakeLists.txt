add_library("isl_omnetpp" SHARED "")

target_include_directories("isl_omnetpp" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

set(LIBS_TARGET "isl_api")

target_link_libraries("isl_omnetpp" ${LIBS_TARGET})

install(TARGETS "isl_omnetpp" CONFIGURATIONS Release DESTINATION "bin/${PLATFORM_DIRECTORY}")

if(MSVC)
    add_compile_definitions(ISL_OMNETPP_LIB)
endif()

add_subdirectory("include")
add_subdirectory("src")
