#ifndef __FINGER_TREE_SUCC_ARRY
#define __FINGER_TREE_SUCC_ARRY

#include <assert.h>
#include <cstring>

template <typename pointer>
struct Level {
	pointer affix;
	pointer slop;

	inline Level<pointer>
	add(bool left, int state, pointer element) const {
		switch(state) {
			case 0:
				return {.slop = element};
			case 1:
				if(left) {
					return {.affix = element, .slop = this->slop};
				} else {
					return {.affix = this->slop, .slop = element};
				}
			case 2: {
				FingerNode *node;
				if(left) {
					node = new FingerNode(elem, this.affix, this.slop, elem);
				} else {
					node = new FingerNode(this.affix, this.slop, elem);
				}
				return {.affix = node};
			}
			case 3:
				return {.affix = this->affix, .slop = element};
			default: // case 4:
				assert(0 && "I shouldn't be reached");
		}
	}

	inline Level<pointer>
	remove(bool left, int state) {
		switch(state) {
			case 2:
				return {.slop = this->affix};
			case 3:
				if(left) {
					return {.affix = this.affix.middle, .slop = this.affix.right};
				} else {
					return {.affix = this->affix.left, .slop = this.affix.middle};
				}
			case 4:
				return {.affix = this->affix};
			default: // case 0 and case 1
				assert(0 && "I shouldn't be reached");
	}
};

template <typename Bitmask, typename pointer>
class SuccinctArray {
	private:
		const pointer *data;
		const Bitmask schema;

		SuccinctArray(pointer *payload, Bitmask mask): data(payload), schema(mask) {}

	public:
		SuccinctArray(void): data(nullptr), schema() {}

		const SuccinctArray<Bitmask, pointer> *
		append(const SuccinctArray *other) const;

		const SuccinctArray<Bitmask, pointer> *
		setHeadLevel(const int, const pointer affix, const pointer slop) const;

		const Level<pointer>
		getHeadLevel(void) const;

		const int
		getState(int level) const;

		const SuccinctArray<Bitmask, pointer> *
		offset(int level) const;

		static
		const SuccinctArray<Bitmask, pointer>
		fromArray(const pointer *unpacked, const int size);

		//const std::vector<pointer>
		//unpack(void) const;
};

template <typename Bitmask, typename pointer>
const SuccinctArray<Bitmask, pointer>
SuccinctArray<Bitmask, pointer>::fromArray(const pointer *unpacked, const int size) {
#ifdef DEBUG
	assert(sizeof(Bitmask) >= size);
#endif

	if(size == 0) {
		return SuccinctArray<Bitmask, pointer>(NULL, 0);
	}
	Bitmask mask = {0};
	const int single_bit = 0x1;

	for(int i=0; i < size; i++) {
		if(unpacked[i] != nullptr) {
			mask = mask | (single_bit << i);
		}
	}
	// Schema is fixed after this point

	pointer *data = new pointer[__builtin_popcount(mask)];

	int lower = 0;
	for(int i=0; i < size; i++) {
		if(unpacked[i] != nullptr) {
			data[lower] = unpacked[i];
			lower++;
		}
	}

	// Data is fixed after this point
	return SuccinctArray<Bitmask, pointer>(data, mask);
}

//
// Start compile-time bitmask helpers
//

#define slop (0x1)
#define one_affix (0x1 << 1)
#define three_affix (0x1 << 3)

template <typename Bitmask>
// The bitmask for a given state
constexpr Bitmask bitmaskFor(const int state) {
	return (state == 1 ? one_affix : 0) |
		(state == 2 ? (slop | one_affix) : 0) |
		(state == 3 ? three_affix : 0) |
		(state == 4 ? (slop | three_affix) : 0);
}

template <typename Bitmask>
constexpr Bitmask
stateRepeat(const int state) {
	return stateRepeatTimes(bitmaskFor<Bitmask>(state), sizeof(Bitmask) / 3);
}

template <typename Bitmask>
constexpr Bitmask
stateRepeatTimes(const Bitmask state, const int num) {
	return (state << (num * 3)) | stateRepeatTimes(state, num - 1);
}

template <typename Bitmask>
// The bitmask for a given state
constexpr int 
state(const Bitmask bitmask) {
	return (bitmask == one_affix ? 1 : 0) |
		(bitmask == (slop | one_affix) ? 2 : 0) |
		(bitmask == three_affix ? 3 : 0) |
		(bitmask == (slop | three_affix) ? 4 : 0);
}

#undef slop
#undef one_affix
#undef three_affix

//
// End compile-time bitmask helpers
//
template <typename Bitmask, typename pointer>
const SuccinctArray<Bitmask, pointer> *
SuccinctArray<Bitmask, pointer>::offset(int level) const {
	const Bitmask suffixMask = this->schema >> (level * 3);
	const pointer *suffixData = &this->data[level * 3];

	return new SuccinctArray(suffixMask, suffixData);
}

template <typename Bitmask, typename pointer>
const SuccinctArray<Bitmask, pointer> *
SuccinctArray<Bitmask, pointer>::append(const SuccinctArray *other) const {
	// shift bitmask by size of schema
	const int thisSize = __builtin_popcount(this->schema);
	// round up the number of levels
	const int levels = (thisSize / 3) + (3 * !!(thisSize % 3));
	const int shiftedOther = other->schema << (levels * 3);
	// and with schema to get bitmask
	const int newMask = shiftedOther & this->schema;

	const int thatSize = __builtin_popcount(other->schema);

	pointer newData[] = new pointer[thisSize + thatSize];
	// memcpy prefix
	memcpy(newData, this->data, thisSize * sizeof(pointer));
	// memcpy suffix
	memcpy(&newData[thisSize], other->data, thatSize * sizeof(pointer));
}

// The number of references the level
// contains if it is in state "state"
constexpr int footprint(const int state) {
	// If 0, return 0
	// else return 2 if the value is even
	// and 1 otherwise
	return (state ? ((state % 2 == 0) ? 2 : 1) : 0);
}

template <typename Bitmask, typename pointer>
const inline SuccinctArray<Bitmask, pointer> *
SuccinctArray<Bitmask, pointer>::parallelOverflow
(const pointer elem, const bool left) const {
	const Bitmask overflowMask = stateRepeat<Bitmask>(4);
	const numOverflowLevels = __builtin_clz(overflow_mask ^ this->schema) / 3;
	const Bitmask afterOverflowMask =
		stateRepeatFor<Bitmask>(bitmaskFor<Bitmask>(state), numOverflowLevels);
	
	// Note zero indexing
	const Level nonOverflowState = this->getState(numOverflowLevels);
	const Level nonOverflow = this->getLevel(nonOverflowState numOverflowLevels);
	const Level nonFuture = nonOverflow.add(left, nonOverflowState, elem);

	// Note: the new element becomes the level 0 slop,
	// level N slop becomes level N 1-affix
	// level N affix becomes level N+1 slop
	//
	// This means that we can memcpy the data segment
	// of overflowing levels after the element, and then
	// handle the one level after that which doesn't overflow
	// and then memcpy the rest
}

// Observation: we only modify head except for
// in overflow, and in that case we can append
// the non-overflowing case onwards, with a simple
// memcpy and bitshift
template <typename Bitmask, typename pointer>
const inline SuccinctArray<Bitmask, pointer> *
SuccinctArray<Bitmask, pointer>::setHeadLevel
(const int state, const pointer affix, const pointer slop) const {

	const int total_size = __builtin_popcount(this->schema);
	const int upper_start = __builtin_popcount(this->schema << 3);
	const int upper_size = total_size - upper_start;
	const Bitmask levelMask = bitmaskFor<Bitmask>(state);

	const char toSkip = __builtin_popcount(this->schema & 0x7);
	const int toCopy = footprint(state);

	pointer *newVersion = new pointer[total_size - toSkip + toCopy];
	if(slop){
		newVersion[0] = slop;
	}
	if(affix){
		newVersion[slop != NULL ? 1 : 0] = affix;
	}
	memcpy(&newVersion[toCopy], &this->data[toSkip], upper_size * sizeof(pointer));

	// Mask out lower bits
	const Bitmask withoutLower = (schema & ~0x7);
	const Bitmask newMask = withoutLower | levelMask;

	return new SuccinctArray<Bitmask, pointer>(newVersion, newMask);
}

template <typename Bitmask, typename pointer>
const int
SuccinctArray<Bitmask, pointer>::getState(int level) const {
	// Shift out lower levels, mask to move current level to head
	const Bitmask base = (schema >> (level * 3)) & 0x7;
	const int three_affix = (base & (0x1 << 2)) ? 3 : 0;
	const int one_affix = (base & (0x1 << 1))  ? 1 : 0;
	const int slop = (base & 0x1) ? 1 : 0;
	return slop + one_affix + three_affix;
}

template <typename Bitmask, typename pointer>
const inline Level<pointer>
SuccinctArray<Bitmask, pointer>::getLevel(int level, int state) const {
	int tmp_off;
	if(level != 0) {
		tmp_off = __builtin_popcount(this->schema & (~0x1 << level * 3));
	} else {
		tmp_off = 0;
	}

	const int offset = tmp_off;

	switch(state) {
		case 0:
			return {0};
		case 1:
			// Left is affix, right is slot
			// This follows bitmask ordering, not array
			// ordering
			return {.slop = this->data[0 + offset]};
		case 2:
		case 4:
			return {.affix = this->data[1 + offset], .slop = this->data[0 + offset]};
		case 3:
			return {.affix = this->data[0 + offset]};
		default:
			assert(0 && "Not reached");
			return {0};
	}
}

#endif
