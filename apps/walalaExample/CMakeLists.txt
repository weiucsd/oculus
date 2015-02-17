cmake_minimum_required(VERSION 2.8)
project (walalaExample)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(PROJECT_SRCS 
${PROJECT_SOURCE_DIR}/walalaExample.cpp
)

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories(../../modules/walala/inc/)

find_package( OpenCV REQUIRED )
find_package( OculusSDK REQUIRED )
find_package( OvrvisionSDK REQUIRED )

include_directories("${OpenCV_INCLUDE_DIRS}")
include_directories(${OCULUS_SDK_INCLUDE_DIRS})
include_directories(${OVRVISION_SDK_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${PROJECT_SRCS})

target_link_libraries (${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries (${PROJECT_NAME} ${OCULUS_SDK_LIBRARIES})
target_link_libraries (${PROJECT_NAME} ${OVRVISION_SDK_LIBRARIES})
target_link_libraries (${PROJECT_NAME} walala)
