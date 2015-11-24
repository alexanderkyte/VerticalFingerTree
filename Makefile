
all: test_array
	./test_array

test_array: test_array.cpp Makefile succ_array.hpp
	clang++ -std=c++11 -ggdb test_array.cpp -o test_array
