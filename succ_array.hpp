#ifndef __FINGER_TREE_SUCC_ARRY
#define __FINGER_TREE_SUCC_ARRY

#include <assert.h>
#include <cstring>

template <typename pointer>
struct Level {
	pointer affix;
	pointer slop;
};

template <typename Bitmask, typename pointer>
class SuccinctArray {
	private:
		const pointer *data;
		const Bitmask schema;

		SuccinctArray(pointer *payload, Bitmask mask): data(payload), schema(mask) {}

	public:
		const SuccinctArray *
		append(const SuccinctArray *other) const;

		template <int state>
		const SuccinctArray<Bitmask, pointer> *
		setHeadLevel(const Level<pointer> added) const;

		const Level<pointer>
		getHeadLevel(void) const;

		const int
		getState(int level) const;

		const SuccinctArray *
		offset(int level) const;

		static
		const SuccinctArray *
		fromArray(const pointer *unpacked, const int size);

		//const std::vector<pointer>
		//unpack(void) const;
};

template <typename Bitmask, typename pointer>
const SuccinctArray<Bitmask, pointer> *
SuccinctArray<Bitmask, pointer>::fromArray(const pointer *unpacked, const int size) {
#ifdef DEBUG
	assert(sizeof(Bitmask) >= size);
#endif

	if(size == 0) {
		return new SuccinctArray<Bitmask, pointer>(NULL, 0);
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

	return new SuccinctArray<Bitmask, pointer>(data, mask);
}

//template <typename Bitmask, typename pointer>
//const std::vector<pointer>
//SuccinctArray<Bitmask, pointer>::unpack(void) const {
	//const int totalSize = this->schema ? __builtin_clz(this->schema) : 0;
	//const int packedSize = __builtin_popcount(this->schema);

	//auto unpacked = new std::vector<pointer>(totalSize, nullptr);

	//for(int i=0; i < totalSize; i++) {
		//// If this bit is set
		//if(((this->schema >> i) & 0x1) == 0x1) {
			//unpacked[i] = this->data[bottom_cursor];
			//bottom_cursor++;
		//}
	//}

	//return unpacked;
//}

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

//
//
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
// The bitmask for a given state
constexpr int state(const Bitmask bitmask) {
	return (bitmask == one_affix ? 1 : 0) |
		(bitmask == (slop | one_affix) ? 2 : 0) |
		(bitmask == three_affix ? 3 : 0) |
		(bitmask == (slop | three_affix) ? 4 : 0);
}

#undef slop
#undef one_affix
#undef three_affix

//
//
//

// Observation: we only modify head except for
// in overflow, and in that case we can append
// the non-overflowing case onwards, with a simple
// memcpy and bitshift
template <typename Bitmask, typename pointer>
template <const int state>
const SuccinctArray<Bitmask, pointer> *
SuccinctArray<Bitmask, pointer>::setHeadLevel
(const Level<pointer> added) const {

	const int total_size = __builtin_popcount(this->schema);
	const int upper_start = __builtin_popcount(this->schema << 3);
	const int upper_size = total_size - upper_start;
	const Bitmask levelMask = bitmaskFor<Bitmask>(state);

	const char toSkip = __builtin_popcount(this->schema & 0x7);
	const int toCopy = footprint(state);

	pointer *newVersion = new pointer[total_size - toSkip + toCopy];
	if(added.slop){
		newVersion[0] = added.slop;
	}
	if(added.affix){
		newVersion[added.slop != NULL ? 1 : 0] = added.affix;
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
	const Bitmask base = (schema >> level) & 0x7;
	const int three_affix = (base & (0x1 << 2)) ? 3 : 0;
	const int one_affix = (base & (0x1 << 1))  ? 1 : 0;
	const int slop = (base & 0x1) ? 1 : 0;
	return slop + one_affix + three_affix;
}

template <typename Bitmask, typename pointer>
const Level<pointer>
SuccinctArray<Bitmask, pointer>::getHeadLevel(void) const {
	const int state = getState(0);

	switch(state) {
		case 0:
			return {0};
		case 1:
			// Left is affix, right is slot
			// This follows bitmask ordering, not array
			// ordering
			return {.affix = nullptr, .slop = this->data[0]};
		case 2:
		case 4:
			return {.affix = this->data[1], .slop = this->data[0]};
		case 3:
			return {.affix = this->data[0], .slop = nullptr};
		default:
			assert(0 || "Not reached");
			return {0};
	}
}

#endif
