cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# set the project name
project(Bou
	VERSION 0.0.1
	LANGUAGES CXX C ASM)

set(sources 
	ast_node.cc
	compiler.cc
	darray.cc
	debug.cc
	elf.cc
	parser.cc
	scanner.cc
  )
# add the executable
add_executable(bou
	bou.cc
  ${sources}
  )

add_executable(test
  test_darray.cc
  darray.cc
	)
