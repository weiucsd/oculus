cmake_minimum_required(VERSION 2.8)
project (walala)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(PROJECT_SRCS 
${PROJECT_SOURCE_DIR}/walala.cpp
${PROJECT_INCLUDE_DIR}/walala.h

${PROJECT_SOURCE_DIR}/hands.cpp
${PROJECT_INCLUDE_DIR}/hands.h
)

include_directories("${PROJECT_INCLUDE_DIR}")

find_package( OpenCV REQUIRED )

if(MSVC)
    add_library(${PROJECT_NAME} STATIC ${PROJECT_SRCS})
else()
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS})
endif()

target_link_libraries (${PROJECT_NAME} ${OpenCV_LIBS})