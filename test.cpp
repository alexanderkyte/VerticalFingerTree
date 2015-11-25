#include "finger_tree.hpp"
#include <stdint.h>

void
pushFingerInt(void)
{
	auto init = new FingerTree<int64_t>();
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
