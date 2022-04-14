cmake_minimum_required(VERSION 3.10)

# set the project name
project(Bou
	VERSION 0.0.1
	LANGUAGES CXX C ASM)

# add the executable
add_executable(bou
	ast_node.cc
	bou.cc
	compiler.cc
	darray.cc
	debug.cc
	elf.cc
	parser.cc
	scanner.cc
	)
