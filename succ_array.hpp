#ifndef __FINGER_TREE_SUCC_ARRY
#define __FINGER_TREE_SUCC_ARRY

#include <assert.h>
#include <cstring>
#include "level.hpp"

template <typename Bitmask, typename pointer>
class SuccinctArray {
	typedef Level<Bitmask, pointer> LevelType;

  private:
    const pointer *data;
    const Bitmask schema;

    SuccinctArray(Bitmask mask, pointer *payload): data(payload), schema(mask) {}

  public:
    SuccinctArray(void): data(nullptr), schema() {}

    const inline SuccinctArray<Bitmask, pointer> *
    append(const SuccinctArray *other) const;

    const inline SuccinctArray<Bitmask, pointer> *
    setHeadLevel(const int, const pointer affix, const pointer slop) const;

    const inline LevelType
    getLevel(int level, int state) const;

    const inline int
    getState(int level) const;

    const inline SuccinctArray<Bitmask, pointer> *
    offset(int level) const;

    static
    const inline SuccinctArray<Bitmask, pointer>
    fromArray(const pointer *unpacked, const int size);

    const inline SuccinctArray<Bitmask, pointer> *
    parallelUnderflow
    (const bool left) const;

    const inline SuccinctArray<Bitmask, pointer> *
    parallelOverflow
    (const bool left, const pointer elem) const;

    //const std::vector<pointer>
    //unpack(void) const;
};

template <typename Bitmask, typename pointer>
const inline SuccinctArray<Bitmask, pointer>
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
  return SuccinctArray<Bitmask, pointer>(mask, data);
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
stateRepeatTimes(const Bitmask state, const int num) {
  return (state << (num * 3)) | stateRepeatTimes(state, num - 1);
}

template <typename Bitmask>
constexpr Bitmask
stateRepeat(const int state) {
  return stateRepeatTimes(bitmaskFor<Bitmask>(state), sizeof(Bitmask) / 3);
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
SuccinctArray<Bitmask, pointer>::parallelUnderflow
(const bool left) const {
  // Mask to check for overflow
  const Bitmask underflowMask = stateRepeat<Bitmask>(4);
  const int numUnderflowLevels = __builtin_clz(underflowMask ^ this->schema) / 3;

  // Note zero indexing
  const LevelType nonUnderflowState = this->getState(numUnderflowLevels);
  const LevelType nonUnderflow = this->getLevel(numUnderflowLevels, nonUnderflowState);
  const LevelType futureNonUnderflow = nonUnderflow.remove(left, nonUnderflowState);

  // All levels that previously had state 1 will now have 3
  const Bitmask afterUnderflowMask =
    stateRepeatTimes<Bitmask>(bitmaskFor<Bitmask>(3), numUnderflowLevels);
  const Bitmask futureNonUnderflowMask = futureNonUnderflow->mask() << numUnderflowLevels;
  const Bitmask unaffected = this->schema & (~0x0 << numUnderflowLevels);
  const Bitmask futureMask = afterUnderflowMask | futureNonUnderflowMask | unaffected;

  // Note: We pop one element from the front of the data
  // For underflows:
  // level N+1 slop becomes level N 3-affix
  // level N affix becomes level N+1 slop.
  // We aren't actually moving any bits, just reinterpreting them
  //
  // This means that we can memcpy:
  //   the underflowing levels, skipping first slop
  //   the first non-underflowing level, with one element removed
  //   the elements after the first non-underflowing level
  const int numBefore = __builtin_popcount(this->schema & (~0x0 >> numUnderflowLevels)) - 1;
  const int numAfter = __builtin_popcount(unaffected);
  const int slopFootprint = (futureNonUnderflow->slop != nullptr) ? 1 : 0;
  const int affixFootprint = (futureNonUnderflow->affix != nullptr) ? 1 : 0;
  const int unaffectedCount = __builtin_popcount(unaffected);
  const int totalCount = numBefore + numAfter + slopFootprint + affixFootprint + unaffectedCount;

  pointer *newData = new pointer[totalCount];

  // The underflowing levels
  memcpy(newData, &this->data[1], numBefore * sizeof(pointer));

  // The first non-underflowing level, with one element removed
  if(slopFootprint != 0) {
    newData[numBefore] = futureNonUnderflow->slop;
  }
  if(futureNonUnderflow->affix) {
    newData[numBefore + slopFootprint] = futureNonUnderflow->affix;
  }

  // The elements after the first non-underflowing level
  const int copied = numBefore + slopFootprint + affixFootprint;
  memcpy(&newData[copied], this->data[numBefore], unaffectedCount * sizeof(pointer));

  return new SuccinctArray<Bitmask, pointer>(futureMask, newData);
}



template <typename Bitmask, typename pointer>
const inline SuccinctArray<Bitmask, pointer> *
SuccinctArray<Bitmask, pointer>::parallelOverflow
(const bool left, const pointer elem) const {
  // Mask to check for overflow
  const Bitmask overflowMask = stateRepeat<Bitmask>(4);
  const int numOverflowLevels = __builtin_clz(overflowMask ^ this->schema) / 3;

  // Note zero indexing
  const int nonOverflowState = this->getState(numOverflowLevels);
  const LevelType nonOverflow = this->getLevel(numOverflowLevels, nonOverflowState);
  const LevelType futureNonOverflow = nonOverflow.add(left, elem);

  // All levels that previously had state 4 will now have 2
  const Bitmask afterOverflowMask =
    stateRepeatTimes<Bitmask>(bitmaskFor<Bitmask>(2), numOverflowLevels);
  const Bitmask futureNonOverflowMask = futureNonOverflow.mask() << numOverflowLevels;
  const Bitmask unaffected = this->schema & (~0x0 << numOverflowLevels);
  const Bitmask futureMask = afterOverflowMask | futureNonOverflowMask | unaffected;

  // Note: the new element becomes the level 0 slop,
  // For overflows:
  // level N slop becomes level N 1-affix
  // level N affix becomes level N+1 slop.
  // We aren't actually moving any bits, just reinterpreting them
  //
  // This means that we can memcpy:
  //   the new element
  //   the overflowing levels
  //   the first non-overflowing level, with one element added
  //   the elements after the first non-overflowing level
  const int numBefore = __builtin_popcount(this->schema & (~0x0 >> numOverflowLevels));
  const int numAfter = __builtin_popcount(unaffected);
  const int slopFootprint = (futureNonOverflow.slop != nullptr) ? 1 : 0;
  const int affixFootprint = (futureNonOverflow.affix != nullptr) ? 1 : 0;
  const int unaffectedCount = __builtin_popcount(unaffected);
  const int totalCount = numBefore + numAfter + slopFootprint + affixFootprint + unaffectedCount + 1;

  pointer *newData = new pointer[totalCount];

  // The new element
  newData[0] = elem;

  // The overflowing levels
  memcpy(&newData[1], this->data, numBefore * sizeof(pointer));

  // The first non-overflowing level, with one element added
  if(slopFootprint != 0) {
    newData[1 + numBefore] = futureNonOverflow.slop;
  }
  if(futureNonOverflow.affix) {
    newData[1 + numBefore + slopFootprint] = futureNonOverflow.affix;
  }

  // The elements after the first non-overflowing level
  const int copied = 1 + numBefore + slopFootprint + affixFootprint;
  memcpy(&newData[copied], this->data[numBefore], unaffectedCount * sizeof(pointer));

  return new SuccinctArray<Bitmask, pointer>(futureMask, newData);
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

  return new SuccinctArray<Bitmask, pointer>(newMask, newVersion);
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
inline const Level<Bitmask, pointer>
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
      return LevelType(state, nullptr, nullptr);
    case 1:
      // Left is affix, right is slot
      // This follows bitmask ordering, not array
      // ordering
      return LevelType(state, nullptr, this->data[0 + offset]);
    case 2:
    case 4:
      return LevelType(state, this->data[1 + offset], this->data[0 + offset]);
    case 3:
      return LevelType(state, this->data[0 + offset], nullptr);
  }

  assert(0 && "Not reached");
  return LevelType(state, nullptr, nullptr);
}

#endif
