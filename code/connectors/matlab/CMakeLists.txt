find_package(Matlab COMPONENTS MX_LIBRARY)

if(Matlab_FOUND)
    add_library("isl_matlab" SHARED "")

    target_include_directories("isl_matlab" PUBLIC
        ${Matlab_INCLUDE_DIRS}
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:inc>"
    )

    target_link_directories("isl_matlab" PUBLIC)

    set(LIBS_TARGET "isl_api")

    target_link_libraries("isl_matlab" ${LIBS_TARGET})

    install(TARGETS "isl_matlab" RUNTIME DESTINATION "bin/${PLATFORM_DIRECTORY}")

    if(MSVC)
        add_compile_definitions(ISL_MATLAB_LIB)
    endif()

    add_subdirectory("include")
    add_subdirectory("src")

    add_subdirectory("mex")
else()
    message(WARNING "Matlab not found. Failed to build the isl_matlab library.")
endif()
