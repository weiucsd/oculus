cmake_minimum_required(VERSION 2.8)
project (Oculus)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(PROJECT_SRCS 
${PROJECT_SOURCE_DIR}/OculusBase.cpp
${PROJECT_SOURCE_DIR}/OculusScene.cpp
${PROJECT_SOURCE_DIR}/OculusSampleScene.cpp
${PROJECT_INCLUDE_DIR}/OculusBase.h
${PROJECT_INCLUDE_DIR}/OculusScene.h
${PROJECT_INCLUDE_DIR}/OculusSampleScene.h
)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-qual")
endif()

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories(../walala/inc/)
include_directories(../OvrModule/inc/)

find_package( OculusSDK REQUIRED )
find_package( GLEW REQUIRED )
find_package( OpenCV REQUIRED )
find_package( OvrvisionSDK REQUIRED )

if(MSVC)
	find_package( GLFW REQUIRED )
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	find_package( PkgConfig REQUIRED)
	pkg_search_module( GLFW REQUIRED glfw3 )
endif()

include_directories(${OCULUS_SDK_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_PATH})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories("${OpenCV_INCLUDE_DIRS}")
include_directories(${OVRVISION_SDK_INCLUDE_DIRS})

if(MSVC)
    add_library(${PROJECT_NAME} STATIC ${PROJECT_SRCS})
else()
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS})
endif()

target_link_libraries (${PROJECT_NAME} ${OCULUS_SDK_LIBRARIES})
target_link_libraries (${PROJECT_NAME} ${GLEW_LIBRARY})
target_link_libraries (${PROJECT_NAME} ${GLFW_STATIC_LIBRARIES})
target_link_libraries (${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries (${PROJECT_NAME} walala)
target_link_libraries (${PROJECT_NAME} OvrModule)
target_link_libraries (${PROJECT_NAME} ${OVRVISION_SDK_LIBRARIES})

if(MSVC)
	target_link_libraries (${PROJECT_NAME} winmm)
endif()