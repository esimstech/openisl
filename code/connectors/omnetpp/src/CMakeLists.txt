set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_omnetpp_global.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_omnetpp.cpp"
)

if(FILES)
    target_sources("isl_omnetpp" PRIVATE ${FILES})
endif()