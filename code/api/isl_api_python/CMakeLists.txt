find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Create a virtual environment
execute_process(COMMAND "${Python3_EXECUTABLE}"
    "-m" "venv" "${CMAKE_CURRENT_BINARY_DIR}/pyenv"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Work from now in the virtual environment
set(ENV{VIRTUAL_ENV} "${CMAKE_CURRENT_BINARY_DIR}/pyenv")

# Search first for the virtual environment
set(Python3_FIND_VIRTUALENV FIRST)

# Unset the current python executable
unset(Python3_EXECUTABLE)

# Find again the package
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Install the wheel package to build our library
execute_process(COMMAND "${Python3_EXECUTABLE}"
    "-m" "pip" "install" "wheel"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Install the setuptools package to build our library
execute_process(COMMAND "${Python3_EXECUTABLE}"
    "-m" "pip" "install" "setuptools"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Build the ISL library...
# ... as a source distribution ...
execute_process(COMMAND "${Python3_EXECUTABLE}"
    "setup.py" "sdist" "--formats=zip" "--dist-dir=${CMAKE_CURRENT_BINARY_DIR}/dist"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ... or as a wheel distribution
execute_process(COMMAND "${Python3_EXECUTABLE}"
    "setup.py" "bdist_wheel" "--dist-dir=${CMAKE_CURRENT_BINARY_DIR}/dist"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Clean the src directory
if(WIN32)
    execute_process(COMMAND "00_clean_folder.bat"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
else()
    execute_process(COMMAND "sh 00_clean_folder.sh"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
endif()

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dist" DESTINATION "connect/python" FILES_MATCHING PATTERN "OpenISL*")

add_subdirectory("resources")
