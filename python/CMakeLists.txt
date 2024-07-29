set(WRAP_FILE XtbPluginWrapper.cpp)
set(MODULE_NAME openmmxtb)

# Copy data files.

file(GLOB_RECURSE STAGING_INPUT_FILES1 RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/openmmxtb/*"
)
foreach(file ${STAGING_INPUT_FILES1})
    set(STAGING_INPUT_FILES ${STAGING_INPUT_FILES} "${file}")
endforeach()
foreach(file ${STAGING_INPUT_FILES})
    set(infile "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    set(outfile "${CMAKE_BINARY_DIR}/python/${file}")
    add_custom_command(
        OUTPUT "${outfile}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${infile}" "${outfile}"
        DEPENDS "${infile}"
        COMMENT "CMake-copying file ${infile} to ${outfile}")
    set(STAGING_OUTPUT_FILES ${STAGING_OUTPUT_FILES} "${outfile}")
endforeach()

# Execute SWIG to generate source code for the Python module.

add_custom_command(
    OUTPUT "${WRAP_FILE}"
    COMMAND "${SWIG_EXECUTABLE}"
        -python -c++
        -o "${WRAP_FILE}"
        -outdir "${CMAKE_BINARY_DIR}/python/openmmxtb"
        "-I${OPENMM_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/xtbplugin.i"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/xtbplugin.i"
        ${STAGING_OUTPUT_FILES}
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
