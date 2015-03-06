cmake_minimum_required(VERSION 2.8)
project (KalmanFilter)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(PROJECT_SRCS 
${PROJECT_SOURCE_DIR}/KalmanFilter.cpp
)

include_directories("${PROJECT_INCLUDE_DIR}")

find_package( OpenCV REQUIRED )

include_directories("${OpenCV_INCLUDE_DIRS}")

add_executable(${PROJECT_NAME} ${PROJECT_SRCS})

target_link_libraries (${PROJECT_NAME} ${OpenCV_LIBS})

