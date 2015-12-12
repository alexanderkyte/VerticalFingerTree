
all: test
	./test

test: test.cpp Makefile finger_tree.hpp
	clang++ -std=c++11 -ggdb test.cpp -o test

clean:
	- rm -rf *.gch *.dSYM *.o test test_array

