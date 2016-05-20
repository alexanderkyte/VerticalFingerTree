
all: test
	./test

test: test.cpp Makefile random_access_test.cpp
	clang++ -std=c++11 -ggdb random_access_test.cpp -o random_access_test

clean:
	- rm -rf *.gch *.dSYM *.o test test_array

