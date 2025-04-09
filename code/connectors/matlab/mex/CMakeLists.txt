matlab_get_mex_suffix(${Matlab_ROOT_DIR} MATLAB_EXTENSION)

set(MEX_SOURCE_NAMES
"ISLInitialize"
"ISLGetDouble"
"ISLGetDoubleLast"
"ISLGetDoubleEvt"
"ISLGetInteger"
"ISLSetDouble"
"ISLSetDoubleEvt"
"ISLSetInteger"
"ISLDisconnect"
)

if(MSVC)
    remove_definitions(-DISL_MATLAB_LIB)
endif()

set(HAS_MEX_COMPILED FALSE)
if("${MATLAB_EXTENSION}" STREQUAL "mexw64" AND "${PLATFORM_DIRECTORY}" STREQUAL "x64")
    set(HAS_MEX_COMPILED TRUE)
elseif("${MATLAB_EXTENSION}" STREQUAL "mexw32" AND "${PLATFORM_DIRECTORY}" STREQUAL "x86")
    set(HAS_MEX_COMPILED TRUE)
endif()

if(${HAS_MEX_COMPILED})
    foreach(MEX_SOURCE_NAME ${MEX_SOURCE_NAMES})
        matlab_add_mex(NAME "${MEX_SOURCE_NAME}" SRC "${MEX_SOURCE_NAME}.cpp" LINK_TO "isl_matlab")
        set_target_properties("${MEX_SOURCE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    endforeach()

    foreach(MEX_SOURCE_NAME ${MEX_SOURCE_NAMES})
        install(TARGETS "${MEX_SOURCE_NAME}" RUNTIME DESTINATION "bin/${PLATFORM_DIRECTORY}")
    endforeach()
endif()
