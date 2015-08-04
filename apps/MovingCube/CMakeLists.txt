cmake_minimum_required(VERSION 2.8)
project (MovingCube)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(PROJECT_SRCS 
${PROJECT_SOURCE_DIR}/main.cpp
)

IF (WIN32)
	SET(CMAKE_DEBUG_POSTFIX d)
ENDIF()

find_package( OculusSDK REQUIRED )
find_package( PCSDK REQUIRED )

MESSAGE(${OCULUS_SDK_LIBRARY})

include_directories(${OCULUS_SDK_INCLUDE_DIRS})
include_directories(${PROJECT_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${PROJECT_SRCS})

target_link_libraries (${PROJECT_NAME} ${OCULUS_SDK_LIBRARIES})
target_link_libraries (${PROJECT_NAME} ${PCSDK_LIBRARIES})