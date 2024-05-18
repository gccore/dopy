cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_MAJOR_VERSION 0)
set(CMAKE_MINOR_VERSION 0)
set(CMAKE_PATCH_VERSION 0)

project(dopy LANGUAGES CXX VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION})

file(GLOB source_files
  source/*.cc
)
file(GLOB header_files
  include/${PROJECT_NAME}/*.hh
)

add_executable(${PROJECT_NAME}
  ${source_files}
  ${header_files}
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED on
    CXX_EXTENSIONS off
    VERSION ${PROJECT_VERSION}
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
