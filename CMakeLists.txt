cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED PROJECT_NAME)
  set(NOT_SUBPROJECT ON)
else()
  set(NOT_SUBPROJECT OFF)
endif()

project(ordered_binary_trees)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_library(ordered_binary_trees OBJECT)

set_target_properties(ordered_binary_trees PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ordered_binary_trees PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(ordered_binary_trees PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ordered_binary_trees/assert.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ordered_binary_trees/basic_tree_impl.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ordered_binary_trees/managed_tree.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ordered_binary_trees/ordered_binary_tree.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ordered_binary_trees/ordered_binary_tree_iterator.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ordered_binary_trees/ordered_binary_tree_node.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ordered_binary_trees/splay_tree_impl.hpp"
)

if(NOT_SUBPROJECT)

  # Doxygen

  find_package(Doxygen OPTIONAL_COMPONENTS dot)
  if(DOXYGEN_FOUND)
    doxygen_add_docs(doxygen "${CMAKE_CURRENT_SOURCE_DIR}/include"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")
  else()
    message("Install Doxygen to make documentation available. "
      "See https://github.com/doxygen/doxygen for more information.")
  endif()

  # Tests are only included if this is not a subproject and Catch2 submodule is
  # present.

  set(catch2_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/Catch2")

  if(EXISTS "${catch2_SOURCE_DIR}/CMakeLists.txt")
    include(CTest)

    add_subdirectory("${catch2_SOURCE_DIR}")
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
    include("${catch2_SOURCE_DIR}/extras/Catch.cmake")

    add_subdirectory(tests)
  else()
    message("Catch2 submodule has not been initialized -- Testing is disabled.")
    message("To make Catch2 available, use 'git submodule update --init'")
  endif()

endif()
