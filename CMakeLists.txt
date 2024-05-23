cmake_minimum_required(VERSION 3.17)
project(two-side-bfs)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")

enable_language(C)

if(BUILD_FROM_SOURCES)
    message(STATUS "Building from sources is on")

    add_subdirectory(vendor/GraphBLAS)
    add_subdirectory(vendor/LAGraph)
else()
    find_package ( GraphBLAS REQUIRED )
    find_package ( LAGraph REQUIRED )
endif()

add_executable(two-side-bfs
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/two_side_bfs.c)

set_target_properties ( PROPERTIES
    C_STANDARD_REQUIRED ON
    C_STANDARD 11 )

#if (GraphBLAS_FOUND AND LAGraph_FOUND)
    target_link_libraries(two-side-bfs GraphBLAS LAGraph)
    target_include_directories(two-side-bfs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
        #${LAGRAPH_INCLUDE_DIR})
#else()
#    message(FATAL_ERROR "Cannot resolve dependencies")
#endif()
