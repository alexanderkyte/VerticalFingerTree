#include "finger_tree.h"

void
pushInt(void)
{
	FingerTree *ftree = new FingerTree();
	int test = 10;
	ftree.pushLeft(&test);
	int *result = ftree.popLeft();
	assert(result == &test);
}

int 
main(void) 
{
	pushInt();
	return;
}
