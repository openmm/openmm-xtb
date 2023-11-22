set(WRAP_FILE XtbPluginWrapper.cpp)
set(MODULE_NAME openmmxtb)

# Execute SWIG to generate source code for the Python module.

add_custom_command(
    OUTPUT "${WRAP_FILE}"
    COMMAND "${SWIG_EXECUTABLE}"
        -python -c++
        -o "${WRAP_FILE}"
        "-I${OPENMM_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/xtbplugin.i"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/xtbplugin.i"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Compile the Python module.

add_custom_target(PythonInstall DEPENDS "${WRAP_FILE}")
set(OPENMMXTB_HEADER_DIR "${CMAKE_SOURCE_DIR}/openmmapi/include")
set(OPENMMXTB_LIBRARY_DIR "${CMAKE_BINARY_DIR}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
add_custom_command(TARGET PythonInstall
    COMMAND "${PIP_EXECUTABLE}" install .
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
