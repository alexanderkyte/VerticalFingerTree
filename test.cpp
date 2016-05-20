#include "finger_tree.hpp"
#include <stdint.h>

void
pushFingerInt(void)
{
	auto init = new FingerTree<int>();
	for (int i=0; i < 100; i++) {
		init = init.pushLeft (i);
	}
	int test = 10;
	auto filled = init->pushLeft((void **)&test);
	int *result = (int *)filled->peekLeft();
	assert(result == &test);
}

int 
main(void) 
{
	pushFingerInt();
	return 0;
}
