#include "succ_array.hpp"
#include "stdint.h"
#include "assert.h"

typedef const void * pointer;
typedef uint64_t bitmask;

void
pushInt(void)
{
	const int foo = 100;
	const int bar = 30;
	const pointer data[] = {&foo, &bar};
	auto arry = SuccinctArray<bitmask, pointer>::fromArray(nullptr, 0);
	auto next = arry->setHeadLevel<2>(data);
	auto both = next->getHeadLevel();
	assert(std::get<0>(both) == &foo);
}

int 
main(void) 
{
	pushInt();

	return 0;
}
