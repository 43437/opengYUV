cmake_minimum_required(VERSION 2.6)
project(openglyuv)

set(base_dir ${CMAKE_CURRENT_LIST_DIR})

set(src_dir ${base_dir})

file(GLOB src_files ${src_dir}/*.cpp)

set(CMAKE_CXX_FLAGS "-std=c++11")

add_executable(openglyuv ${src_files})

target_link_libraries(openglyuv "GL" "GLEW" "glfw")

#install(TARGETS openglyuv RUNTIME DESTINATION bin)
