cmake_minimum_required(VERSION 3.10)
project(lecture_2)

include_directories(lecture2)

find_package(fmt REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(homework_tools homework/main.cpp homework/tools.cpp)

target_link_libraries(homework_tools PRIVATE ${OpenCV_LIBS} fmt::fmt)
