#ifndef __FINGER_TREE_SUCC_ARRY
#define __FINGER_TREE_SUCC_ARRY
#include <assert.h>

struct Level {
	const pointer affix;
	const pointer slop;
};

struct SuccinctArray {
	const pointer *data;
	const Bitmask schema;
};

const inline SuccinctArray
makeSuccinct(SuccinctArray *self, const pointer *unpacked, const int size);

const inline SuccinctArray
offset(SuccinctArray *self, int level);

const inline SuccinctArray
append(SuccinctArray *self, const SuccinctArray *other);

const inline SuccinctArray
setHeadLevel(SuccinctArray *self, const pointer affix, const pointer slop);

const Level<pointer>
getHeadLevel(SuccinctArray *self, void);

const int
getState(SuccinctArray *self, int level);

#endif
