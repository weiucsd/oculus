# - Find OvrvisionSDK
# Find the native OvrvisionSDK headers and libraries.
#
#  OVRVISION_SDK_INCLUDE_DIRS - where to find ovrvision.h, etc.
#  OVRVISION_SDK_LIBRARIES    - List of libraries when using OvrvisionSDK.
#  OVRVISION_SDK_FOUND        - True if OculusSDK found.
#  OVRVISION_SDK_VERSION      - Version of the OvrvisionSDK if found

IF (DEFINED ENV{OVRVISION_SDK_ROOT_DIR})
    SET(OVRVISION_SDK_ROOT_DIR "$ENV{OVRVISION_SDK_ROOT_DIR}")
ENDIF()
SET(OVRVISION_SDK_ROOT_DIR
    "${OVRVISION_SDK_ROOT_DIR}"
    CACHE
    PATH
    "Root directory to search for OvrvisionSDK")

# Look for the header file.
FIND_PATH(OVRVISION_SDK_INCLUDE_DIRS NAMES ovrvision.h HINTS 
	${OVRVISION_SDK_ROOT_DIR}/include )

# Determine architecture
IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
	IF(UNIX)
		SET(_OVRVISION_SDK_LIB_ARCH "x86_64")
	ENDIF()
	IF(MSVC)
		SET(_OVRVISION_SDK_LIB_ARCH "x64")
	ENDIF()
ELSE()
	IF(UNIX)
		SET(_OVRVISION_SDK_LIB_ARCH "i386")
	ENDIF()
	IF(MSVC)
		SET(_OVRVISION_SDK_LIB_ARCH "x86")
	ENDIF()
ENDIF()

MARK_AS_ADVANCED(_OVRVISION_SDK_LIB_ARCH)

# Append "d" to debug libs on windows platform
IF (WIN32)
	SET(CMAKE_DEBUG_POSTFIX d)
ENDIF()

# Determine the compiler version for Visual Studio
IF (MSVC)
	# Visual Studio 2010
	IF(MSVC10)
		SET(_OVRVISION_MSVC_DIR "VS2010")
	ENDIF()
	# Visual Studio 2012
	IF(MSVC11)
		SET(_OVRVISION_MSVC_DIR "VS2012")
	ENDIF()
	# Visual Studio 2013
	IF(MSVC12)
		SET(_OVRVISION_MSVC_DIR "VS2013")
	ENDIF()
ENDIF()

# Look for the library.
FIND_LIBRARY(OVRVISION_SDK_LIBRARY NAMES ovrvision ovrvision_64 HINTS ${OVRVISION_SDK_ROOT_DIR}/bin/${_OVRVISION_SDK_LIB_ARCH})

#MESSAGE( STATUS "test: " ${OVRVISION_SDK_LIBRARY})

# This will find release lib on Linux if no debug is available - on Linux this is no problem and avoids 
# having to compile in debug when not needed
FIND_LIBRARY(OVRVISION_SDK_LIBRARY_DEBUG NAMES ovrvision${CMAKE_DEBUG_POSTFIX} ovrvision_64${CMAKE_DEBUG_POSTFIX} HINTS 
                                                      ${OVRVISION_SDK_ROOT_DIR}/bin/${_OVRVISION_SDK_LIB_ARCH})

MARK_AS_ADVANCED(OVRVISION_SDK_LIBRARY)
MARK_AS_ADVANCED(OVRVISION_SDK_LIBRARY_DEBUG)

SET(OVRVISION_SDK_LIBRARIES optimized ${OVRVISION_SDK_LIBRARY} debug ${OVRVISION_SDK_LIBRARY_DEBUG})

# handle the QUIETLY and REQUIRED arguments and set OVRVISION_SDK_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OvrvisionSDK DEFAULT_MSG OVRVISION_SDK_LIBRARIES OVRVISION_SDK_INCLUDE_DIRS)

MARK_AS_ADVANCED(OVRVISION_SDK_LIBRARIES OVRVISION_SDK_INCLUDE_DIRS)