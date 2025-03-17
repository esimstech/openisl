add_library("isl_lua" SHARED "")

if("${PLATFORM_DIRECTORY}" STREQUAL "x64")
    set(LUA_BIN_PATH "$ENV{LUA_PATH}/bin.w64")
else()
    set(LUA_BIN_PATH "$ENV{LUA_PATH}/bin.w32")
endif()

if (WIN32)
    file(TO_CMAKE_PATH "${LUA_BIN_PATH}/lua53.dll" LUA_RUNTIME)
endif()

target_include_directories("isl_lua" PUBLIC
    "${LUA_BIN_PATH}/include"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

target_link_directories("isl_lua" PUBLIC "${LUA_BIN_PATH}")

find_library(LUA_LIB "lua53" "${LUA_BIN_PATH}")
set(LIBS_TARGET "isl_api" ${LUA_LIB})
if(NOT MSVC)
    list(APPEND LIBS_TARGET "boost_program_options")
endif()

target_link_libraries("isl_lua" ${LIBS_TARGET})

install(TARGETS "isl_lua" RUNTIME DESTINATION "connect/lua/lib/${PLATFORM_DIRECTORY}")
install(FILES ${LUA_RUNTIME} DESTINATION "connect/lua/lib/${PLATFORM_DIRECTORY}")

if(MSVC)
    add_compile_definitions(ISL_LUA_LIB)
endif()

add_subdirectory("include")
add_subdirectory("src")
add_subdirectory("library")
