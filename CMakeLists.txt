cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# set the project name
project(Bou
	VERSION 0.0.1
	LANGUAGES CXX C ASM)

# single compilation unit build
add_executable(bou
	compilation.cc
  )

add_executable(test_ast
  test_ast.cc
  )
add_executable(test
  test_darray.cc
  darray.cc
  util.cc
  arena.cc
	)
