find_program(ARES_PACKAGE_EXECUTABLE ares-package)
if(NOT ARES_PACKAGE_EXECUTABLE)
    message(FATAL_ERROR "ares-package was not found. Install @webos-tools/cli first.")
endif()

execute_process(
    COMMAND "${ARES_PACKAGE_EXECUTABLE}" "${CPACK_TEMPORARY_DIRECTORY}" -o "${CPACK_PACKAGE_DIRECTORY}"
    RESULT_VARIABLE ARES_PACKAGE_RESULT
)

if(NOT ARES_PACKAGE_RESULT EQUAL 0)
    message(FATAL_ERROR "ares-package failed with exit code ${ARES_PACKAGE_RESULT}")
endif()

