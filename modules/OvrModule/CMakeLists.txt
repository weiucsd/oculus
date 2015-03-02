cmake_minimum_required(VERSION 2.8)
project (OvrModule)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(PROJECT_SRCS 
${PROJECT_SOURCE_DIR}/OvrClass.cpp
${PROJECT_INCLUDE_DIR}/OvrClass.h
)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-qual")
endif()

include_directories("${PROJECT_INCLUDE_DIR}")

find_package( OvrvisionSDK REQUIRED )
find_package( OpenCV REQUIRED )

include_directories(${OVRVISION_SDK_INCLUDE_DIRS})
include_directories("${OpenCV_INCLUDE_DIRS}")

if(MSVC)
    add_library(${PROJECT_NAME} STATIC ${PROJECT_SRCS})
else()
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS})
endif()

target_link_libraries (${PROJECT_NAME} ${OVRVISION_SDK_LIBRARIES})
target_link_libraries (${PROJECT_NAME} ${OpenCV_LIBS})