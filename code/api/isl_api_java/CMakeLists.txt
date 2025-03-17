find_package(Java COMPONENTS Development REQUIRED)
find_package(JNI REQUIRED)

# Compile Java files
set (JAVA_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/java/src/com/openisl/java/CISLConnect.java"
    "${CMAKE_CURRENT_SOURCE_DIR}/java/src/com/openisl/java/CISLData.java"
    "${CMAKE_CURRENT_SOURCE_DIR}/java/src/com/openisl/java/CISLDoubleData.java"
    "${CMAKE_CURRENT_SOURCE_DIR}/java/src/com/openisl/java/CISLIntData.java"
    "${CMAKE_CURRENT_SOURCE_DIR}/java/src/com/openisl/java/CISLStringData.java"
)

foreach(JAVA_SRC_FILE ${JAVA_SRC_FILES})
    execute_process(COMMAND "${Java_JAVAC_EXECUTABLE}"
        "-d" "${CMAKE_CURRENT_BINARY_DIR}/java/bin"
        "${JAVA_SRC_FILE}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/java/src"
    )
endforeach()

# Build the archive
execute_process(COMMAND "${Java_JAR_EXECUTABLE}"
    "cvf" "${CMAKE_CURRENT_BINARY_DIR}/com.openisl.java.jar" "com"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/java/bin"
)

# Build Java header
execute_process(COMMAND "${Java_JAVAC_EXECUTABLE}"
    "-h" "${CMAKE_CURRENT_BINARY_DIR}/java/include"
    "-d" "${CMAKE_CURRENT_BINARY_DIR}/java/bin"
    ${JAVA_SRC_FILES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/java/bin"
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/com.openisl.java.jar" DESTINATION "connect/java")

add_library("isl_java" SHARED "")

target_include_directories("isl_java" PUBLIC
    ${JNI_INCLUDE_DIRS}
    "${CMAKE_CURRENT_BINARY_DIR}/java/include"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:inc>"
)

target_link_libraries("isl_java" "isl_api")

install(TARGETS "isl_java" RUNTIME DESTINATION "bin/${PLATFORM_DIRECTORY}")

add_subdirectory("include")
add_subdirectory("src")
