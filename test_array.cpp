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
	auto arry = SuccinctArray<bitmask, pointer>(nullptr, 0);
	const int head_level_state = 2;
	auto next = arry.setHeadLevel(head_level_state, &affix, &slop);
	const auto both = next->getLevel(0, head_level_state);
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
