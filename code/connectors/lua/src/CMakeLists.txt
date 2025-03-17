set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_lua_global.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_lua.cpp"
)

if(FILES)
    target_sources("isl_lua" PRIVATE ${FILES})
endif()