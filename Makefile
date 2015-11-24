
all: test_array
	./test_array

test_array: test_array.cpp
	clang++ -std=c++11 test_array.cpp -o test_array
