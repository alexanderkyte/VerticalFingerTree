#include "succ_array.hpp"
#include "stdint.h"
#include "assert.h"

#include "iostream"

typedef void const * pointer;
typedef uint64_t bitmask;

void
pushInts(void)
{
	const int affix = 100;
	const int slop = 30;
	SuccinctArray<bitmask, pointer> arry = SuccinctArray<bitmask, pointer>::fromArray(nullptr, 0);
	auto next = arry.setHeadLevel(2, &affix, &slop);
	const Level<pointer> both = next->getHeadLevel();
	const pointer first = both.affix;
	const pointer second = both.slop;
	assert(both.affix == &affix);
	assert(both.slop == &slop);
}

int 
main(void) 
{
	pushInts();

	return 0;
}
