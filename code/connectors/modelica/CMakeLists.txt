add_library("isl_modelica" SHARED "")

set(MODELICA_ROOT_LIB_PATH "connect/openmodelica/OpenISL/Resources/Library")
if("${PLATFORM_DIRECTORY}" STREQUAL "x64")
    set(MODELICA_LIB_PATH "${MODELICA_ROOT_LIB_PATH}/win64")
else()
    set(MODELICA_LIB_PATH "${MODELICA_ROOT_LIB_PATH}/win32")
endif()

target_include_directories("isl_modelica" PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

set(LIBS_TARGET "isl_api")

target_link_libraries("isl_modelica" ${LIBS_TARGET})

install(TARGETS "isl_modelica" CONFIGURATIONS Release DESTINATION "${MODELICA_LIB_PATH}")

if(MSVC)
    add_compile_definitions(ISL_MODELICA_LIB)
endif()

add_subdirectory("include")
add_subdirectory("src")
add_subdirectory("library")
