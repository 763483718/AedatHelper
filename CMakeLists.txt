cmake_minimum_required(VERSION 3.5.1)
project(aedat)

# set (CMAKE_CXX_FLAGS “${CMAKE_CXX_FLAGS} -g”)
set(CMAKE_CXX_STANDARD 11)

set(OpenCV_DIR /home/guanzhou/env_opencv314/share/OpenCV)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(readAedat readAedat.cpp aedat.hpp )

target_link_libraries(readAedat ${OpenCV_LIBS})