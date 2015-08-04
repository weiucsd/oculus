# - Find PCSDK
# Find the native PCSDK headers and libraries.

IF (DEFINED ENV{PCSDK_DIR})
    SET(PCSDK_DIR "$ENV{PCSDK_DIR}")
ENDIF()
SET(PCSDK_DIR
    "${PCSDK_DIR}"
    CACHE
    PATH
    "Root directory to search for PCSDK")

# Add the following directories to C/C++ - General in VS2013
include_directories(${PCSDK_DIR}/include )
include_directories(${PCSDK_DIR}/sample/common/include )

# Determine architecture
IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
	IF(UNIX)
		SET(_PCSDK_Platform_name "x86_64")
	ENDIF()
	IF(MSVC)
		SET(_PCSDK_Platform_name "x64")
	ENDIF()
ELSE()
	IF(UNIX)
		SET(_PCSDK_Platform_name "i386")
	ENDIF()
	IF(MSVC)
		SET(_PCSDK_Platform_name "Win32")
	ENDIF()
ENDIF()

MARK_AS_ADVANCED(_PCSDK_Platform_name)

# Determine the compiler version for Visual Studio
# Notice: There might not be a directory named v120. You may need to create it.
IF (MSVC)
	# Visual Studio 2010
	IF(MSVC10)
		SET(_MSVC_DIR "v100")
	ENDIF()
	# Visual Studio 2012
	IF(MSVC11)
		SET(_MSVC_DIR "v110")
	ENDIF()
	# Visual Studio 2013
	IF(MSVC12)
		SET(_MSVC_DIR "v120")
	ENDIF()
ENDIF()

# Look for the library.
FIND_LIBRARY(PCSDK_LIBRARY_DEBUG NAMES libpxc_d HINTS 
    	${PCSDK_DIR}/lib/${_PCSDK_Platform_name}
	)

FIND_LIBRARY(PCSDK_SAMPLE_LIBRARY_DEBUG NAMES libpxcutils_d HINTS 
    	${PCSDK_DIR}/sample/common/lib/${_PCSDK_Platform_name}/${_MSVC_DIR}
	)

SET(PCSDK_LIBRARIES ${PCSDK_LIBRARY_DEBUG} ${PCSDK_SAMPLE_LIBRARY_DEBUG})

MARK_AS_ADVANCED(PCSDK_LIBRARIES)