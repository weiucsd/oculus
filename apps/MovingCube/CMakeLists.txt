cmake_minimum_required(VERSION 2.8)
project (MovingCube)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(PROJECT_SRCS 
${PROJECT_SOURCE_DIR}/MovingCubeExample.cpp
)

IF (WIN32)
	SET(CMAKE_DEBUG_POSTFIX d)
ENDIF()

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories(../../modules/walala/inc/)
include_directories(../../modules/Oculus/inc/) 
include_directories(../../modules/OvrModule/inc/) 

find_package( OpenCV REQUIRED )
find_package( OculusSDK REQUIRED )
find_package( OvrvisionSDK REQUIRED )
find_package( GLEW REQUIRED )

if(MSVC)
	find_package( GLFW REQUIRED )
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	find_package( PkgConfig REQUIRED)
	pkg_search_module( GLFW REQUIRED glfw3 )
endif()

include_directories("${OpenCV_INCLUDE_DIRS}")
include_directories(${OCULUS_SDK_INCLUDE_DIRS})
include_directories(${OVRVISION_SDK_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_PATH})
include_directories(${GLFW_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${PROJECT_SRCS})

target_link_libraries (${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries (${PROJECT_NAME} ${OCULUS_SDK_LIBRARIES})
#target_link_libraries (${PROJECT_NAME} ${OVRVISION_SDK_LIBRARIES})
target_link_libraries (${PROJECT_NAME} walala)
target_link_libraries (${PROJECT_NAME} ${GLEW_LIBRARY})
target_link_libraries (${PROJECT_NAME} ${GLFW_STATIC_LIBRARIES})
target_link_libraries (${PROJECT_NAME} Oculus)
target_link_libraries (${PROJECT_NAME} OvrModule)
#target_link_libraries (${PROJECT_NAME} OvrModule${CMAKE_DEBUG_POSTFIX})