# Try to find GLFW library and include path.
# Once done this will define
#
# GLFW_FOUND
# GLFW_INCLUDE_DIRS
# GLFW_STATIC_LIBRARIES
#

find_path( GLFW_INCLUDE_DIRS 
    NAMES
        GL/glfw.h
        GLFW/glfw3.h
    PATHS
        "${GLFW_LOCATION}/include"
        "$ENV{GLFW_LOCATION}/include"
        "$ENV{PROGRAMFILES}/GLFW/include"
        "${OPENGL_INCLUDE_DIR}"
        /usr/openwin/share/include
        /usr/openwin/include
        /usr/X11R6/include
        /usr/include/X11
        /opt/graphics/OpenGL/include
        /opt/graphics/OpenGL/contrib/libglfw
        /usr/local/include
        /usr/include/GL
        /usr/include
    DOC 
        "The directory where GL/glfw.h resides"
)

if (WIN32)
    if(CYGWIN)
        find_library( GLFW_glfw_LIBRARY 
            NAMES
                glfw32
            PATHS
                "${GLFW_LOCATION}/lib"
                "${GLFW_LOCATION}/lib/x64"
                "$ENV{GLFW_LOCATION}/lib"
                "${OPENGL_LIBRARY_DIR}"
                /usr/lib
                /usr/lib/w32api
                /usr/local/lib
                /usr/X11R6/lib
            DOC 
                "The GLFW library"
        )
    else()
        find_library( GLFW_glfw_LIBRARY
            NAMES 
                glfw32 
                glfw32s 
                glfw
                glfw3
            PATHS
                "${GLFW_LOCATION}/lib"
                "${GLFW_LOCATION}/lib/x64"
                "${GLFW_LOCATION}/lib-msvc110"
                "$ENV{GLFW_LOCATION}/lib"
                "$ENV{GLFW_LOCATION}/lib/x64"
                IF(MSVC10)
					"$ENV{GLFW_LOCATION}/lib-msvc110"
				ENDIF()
				IF(MSVC11)
					"$ENV{GLFW_LOCATION}/lib-vc2012"
				ENDIF()
				IF(MSVC12)
					"$ENV{GLFW_LOCATION}/lib-vc2013"
				ENDIF()
                "${PROJECT_SOURCE_DIR}/extern/glfw/bin"
                "${PROJECT_SOURCE_DIR}/extern/glfw/lib"
                "$ENV{PROGRAMFILES}/GLFW/lib"
                "${OPENGL_LIBRARY_DIR}"
            DOC 
                "The GLFW library"
        )
    endif()
else ()
    if (APPLE)
        find_library( GLFW_glfw_LIBRARY glfw
            NAMES 
                glfw
                glfw3
            PATHS
                "${GLFW_LOCATION}/lib"
                "${GLFW_LOCATION}/lib/cocoa"
                "$ENV{GLFW_LOCATION}/lib"
                "$ENV{GLFW_LOCATION}/lib/cocoa"
                /usr/local/lib
        )
        set(GLFW_cocoa_LIBRARY "-framework Cocoa" CACHE STRING "Cocoa framework for OSX")
        set(GLFW_corevideo_LIBRARY "-framework CoreVideo" CACHE STRING "CoreVideo framework for OSX")
        set(GLFW_iokit_LIBRARY "-framework IOKit" CACHE STRING "IOKit framework for OSX")
    else ()
        # (*)NIX
        
        find_package(Threads REQUIRED)

        find_package(X11 REQUIRED)
        
        if(NOT X11_Xrandr_FOUND)
            message(FATAL_ERROR "Xrandr library not found - required for GLFW")
        endif()

        if(NOT X11_xf86vmode_FOUND)
            message(FATAL_ERROR "xf86vmode library not found - required for GLFW")
        endif()

        if(NOT X11_Xcursor_FOUND)
            message(FATAL_ERROR "Xcursor library not found - required for GLFW")
        endif()

        list(APPEND GLFW_x11_LIBRARY "${X11_Xrandr_LIB}" "${X11_Xxf86vm_LIB}" "${X11_Xcursor_LIB}" "${CMAKE_THREAD_LIBS_INIT}" -lrt -lXi)

        find_library( GLFW_glfw_LIBRARY
            NAMES 
                glfw
                glfw3
            PATHS
                "${GLFW_LOCATION}/lib"
                "$ENV{GLFW_LOCATION}/lib"
                "${GLFW_LOCATION}/lib/x11"
                "$ENV{GLFW_LOCATION}/lib/x11"
                /usr/lib64
                /usr/lib
                /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                /usr/local/lib64
                /usr/local/lib
                /usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                /usr/openwin/lib
                /usr/X11R6/lib
            DOC 
                "The GLFW library"
        )
    endif (APPLE)
endif (WIN32)

set( GLFW_FOUND "NO" )

if(GLFW_INCLUDE_DIRS)

    if(GLFW_glfw_LIBRARY)
        set( GLFW_STATIC_LIBRARIES "${GLFW_glfw_LIBRARY}"
                            "${GLFW_x11_LIBRARY}"
                            "${GLFW_cocoa_LIBRARY}"
                            "${GLFW_iokit_LIBRARY}"
                            "${GLFW_corevideo_LIBRARY}" )
        set( GLFW_FOUND "YES" )
        set (GLFW_LIBRARY "${GLFW_STATIC_LIBRARIES}")
        set (GLFW_INCLUDE_PATH "${GLFW_INCLUDE_DIRS}")
    endif(GLFW_glfw_LIBRARY)


    # Tease the GLFW_VERSION numbers from the lib headers
    function(parseVersion FILENAME VARNAME)
            
        set(PATTERN "^#define ${VARNAME}.*$")
        
        file(STRINGS "${GLFW_INCLUDE_DIRS}/${FILENAME}" TMP REGEX ${PATTERN})
        
        string(REGEX MATCHALL "[0-9]+" TMP ${TMP})
        
        set(${VARNAME} ${TMP} PARENT_SCOPE)
        
    endfunction()


    if(EXISTS "${GLFW_INCLUDE_DIRS}/GL/glfw.h")

        parseVersion(GL/glfw.h GLFW_VERSION_MAJOR)
        parseVersion(GL/glfw.h GLFW_VERSION_MINOR)
        parseVersion(GL/glfw.h GLFW_VERSION_REVISION)

    elseif(EXISTS "${GLFW_INCLUDE_DIRS}/GLFW/glfw3.h")

        parseVersion(GLFW/glfw3.h GLFW_VERSION_MAJOR)
        parseVersion(GLFW/glfw3.h GLFW_VERSION_MINOR)
        parseVersion(GLFW/glfw3.h GLFW_VERSION_REVISION)
 
    endif()

    if(${GLFW_VERSION_MAJOR} OR ${GLFW_VERSION_MINOR} OR ${GLFW_VERSION_REVISION})
        set(GLFW_VERSION "${GLFW_VERSION_MAJOR}.${GLFW_VERSION_MINOR}.${GLFW_VERSION_REVISION}")
        set(GLFW_VERSION_STRING "${GLFW_VERSION}")
        mark_as_advanced(GLFW_VERSION)
    endif()
    
endif(GLFW_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GLFW 
    REQUIRED_VARS
        GLFW_INCLUDE_DIRS
        GLFW_STATIC_LIBRARIES
    VERSION_VAR
        GLFW_VERSION
)

mark_as_advanced(
  GLFW_INCLUDE_DIRS
  GLFW_STATIC_LIBRARIES
  GLFW_glfw_LIBRARY
  GLFW_cocoa_LIBRARY
)