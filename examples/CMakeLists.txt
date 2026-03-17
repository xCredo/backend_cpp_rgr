cmake_minimum_required(VERSION 3.14)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE SmallVector)
