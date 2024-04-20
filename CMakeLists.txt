cmake_minimum_required(VERSION 3.0)

project(pong VERSION 1.2)

add_executable(pong main.cpp)
target_link_libraries(pong sfml-audio sfml-graphics sfml-window sfml-system)

file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})