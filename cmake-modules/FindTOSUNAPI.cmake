# Try to find TOSUNAPI
# TOSUNAPI_DIR - Hint path to a local build of TOSUNAPI

find_path(TOSUNAPI_INCLUDE_DIR
        NAMES TSCANDef.h
        HINTS ${TOSUNAPI_DIR} ${TOSUNAPI_ROOT} ENV TOSUNAPI_DIR
        PATH_SUFFIXES include inc
)

if (WIN32)
    set(TOSUNAPI_DLL_DIR "${TOSUNAPI_DIR}/lib" CACHE PATH "Path to TOSUNAPI DLL directory")
    find_file(TOSUNAPI_DLL_FILE
    NAMES TSCAN.dll libTSCAN.dll
    HINTS ${TOSUNAPI_DLL_DIR} ${TOSUNAPI_DIR}
    PATH_SUFFIXES lib bin64
    REQUIRED)

    if (TOSUNAPI_DLL_FILE)
        get_filename_component(TOSUNAPI_LIBRARY ${TOSUNAPI_DLL_FILE} ABSOLUTE)
        message(STATUS "Found TOSUNAPI DLL: ${TOSUNAPI_LIBRARY}")
    endif ()
else ()
    find_library(TOSUNAPI_LIBRARY
    NAMES TSCAN.so libTSCANApiOnLinux.so libTSCAN.dylib
    HINTS ${TOSUNAPI_DIR} ${TOSUNAPI_DIR}
    PATH_SUFFIXES lib lib64
    REQUIRED)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TOSUNAPI DEFAULT_MSG
        TOSUNAPI_LIBRARY
)

message("TosunApi headers path: ${TOSUNAPI_INCLUDE_DIR}")
message("TosunApi library path: ${TOSUNAPI_LIBRARY}")

if (TOSUNAPI_FOUND)
    set(TOSUNAPI_INCLUDE_DIRS ${TOSUNAPI_INCLUDE_DIR})
    set(TOSUNAPI_LIBRARIES ${TOSUNAPI_LIBRARY})
endif()

mark_as_advanced(TOSUNAPI_INCLUDE_DIR TOSUNAPI_LIBRARY)

