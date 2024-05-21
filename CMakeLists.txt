cmake_minimum_required(VERSION 3.17)
project(two-side-bfs)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/LAGraph)

enable_language(C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package ( GraphBLAS MODULE REQUIRED )

add_executable(two-side-bfs
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/two_side_bfs.c)

set_target_properties ( PROPERTIES
    C_STANDARD_REQUIRED ON
    C_STANDARD 11 )

target_link_libraries(two-side-bfs GraphBLAS::GraphBLAS LAGraph)
target_include_directories(two-side-bfs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

