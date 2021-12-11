CXX = g++
CXXFLAGS = -I. -g -ggdb -Wall
DEPS = scanner.h token.h debug.h compiler.h parser.h ast_node.h darray.h globals.h elf.h
OBJ = bou.o scanner.o debug.o compiler.o parser.o darray.o elf.o

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

bou: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o 
