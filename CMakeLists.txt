cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_MAJOR_VERSION 0)
set(CMAKE_MINOR_VERSION 1)
set(CMAKE_PATCH_VERSION 0)

project(dopy LANGUAGES CXX VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION})

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -ffunction-sections -fdata-sections")
  set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,--gc-sections")
endif()

file(GLOB source_files
  source/*.cc
)
file(GLOB header_files
  include/${PROJECT_NAME}/*.hh
)

add_subdirectory(third_party/FTXUI)

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
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    -static
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)

install(TARGETS ${PROJECT_NAME})

set(CPACK_GENERATOR "TGZ;ZIP;RPM;DEB;STGZ")
set(CPACK_PACKAGE_VENDOR "GCCORE")
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_NAME}_packages)
set(CPACK_PACKAGE_DESCRIPTION "Simple command-line TUI for packaging AUTOSAR manifests and binaries")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://gccore.github.io")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3")
set(CPACK_PACKAGE_CONTACT "@gccore")

include(CPack)
