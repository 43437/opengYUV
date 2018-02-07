cmake_minimum_required(VERSION 2.6)
project(openglyuv)

add_executable(openglyuv main.cpp)

install(TARGETS openglyuv RUNTIME DESTINATION bin)
