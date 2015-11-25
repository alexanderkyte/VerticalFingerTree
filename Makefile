
all: test_array test
	./test_array
	./test

test: test.cpp Makefile finger_tree.hpp
	clang++ -std=c++11 -ggdb test.cpp -o test

test_array: test_array.cpp Makefile succ_array.hpp
	clang++ -std=c++11 -ggdb test_array.cpp -o test_array
