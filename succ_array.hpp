#ifndef __FINGER_TREE_SUCC_ARRY
#define __FINGER_TREE_SUCC_ARRY

#include <assert.h>
#include <cstring>
#include "level.hpp"

template <typename Left, typename Right, bool which>
union Either {
  Left left;
  Right right;

  const void
  get(std::function left(
}

template <typename Value>
struct HeteroArrayRest {
  const FingerNode<Value> *rest[];

  HeteroArrayTwo(FingerNode<Value> **rest):
    rest(rest); 
}

template <typename Value>
struct HeteroArrayOne: HeteroArrayRest {
  const HeteroArrayRest prefix;
  const Value one;

  HeteroArrayOneFour(const Value one, FingerNode<Value> **rest):
    one(one), rest(rest); 
}

template <typename Value>
struct HeteroArrayTwo: HeteroArrayOne {
  const HeteroArrayOne prefix;
  const Value two;

  HeteroArrayTwo(const Value one, const Value two, FingerNode<Value> **rest):
    one(one), two(two), rest(rest); 
}

template <typename Bitmask, typename Value, typename ArrayType>
class SuccinctArray {
  typedef Level<Bitmask, Value> LevelType;

  private:
    // This contains an array prefix
    // that has unboxed references to values
    const ArrayType contents;
    const Bitmask schema;

    SuccinctArray(Bitmask mask, ArrayType *contents): contents(contents), schema(mask) {}

  public:
    SuccinctArray(void): contents({0}), schema() {}

    ~SuccinctArray(void) {
      for(int i=0; i < __builtin_popcount(schema); i++) {
        delete contents.rest[i];
      }
      delete contents.rest;
      contents.~ArrayType();
    }

    const inline SuccinctArray<Bitmask, Value> *
    append(const SuccinctArray *other) const;

    const inline SuccinctArray<Bitmask, Value> *
    setHeadLevel(const int, const Value affix, const Value slop) const;

    const inline LevelType
    getLevel(int level, int state) const;

    const inline int
    getState(int level) const;

    const inline SuccinctArray<Bitmask, Value> *
    offset(int level) const;

    static
    const inline SuccinctArray<Bitmask, Value>
    fromArray(const Value *unpacked, const int size);

    const inline SuccinctArray<Bitmask, Value> *
    parallelUnderflow
    (const bool left) const;

    const inline SuccinctArray<Bitmask, Value> *
    parallelOverflow
    (const bool left, const Value elem) const;

    const std::pair<bool, Value>
    find(const Measurer measurere, const std::function<bool(Measure)> predicate) const;
};

// Find
template <typename Bitmask>
const std::pair<Measure, Value>
FingerTree<Bitmask>::find(const std::function<Measure(Measure, Measure)> combine,
const std::function<bool(Measure)> predicate) const {
  Measure accum = this->measurer->identity;
  if(this->schema & 0x1) {
    accum = combine(this->array->one);
  }
  if(predicate(accum)) {
    // done
  }
  if(this->schema & 0x2) {
    accum = this->combine(this->array->two);
  }
  if(predicate(accum)) {
    // done
  }
  const int limit = __builtin_popcount(this->schema >> 2);

  // We memoize the accumulation
  if(!this->measureAccum) {
    for(int i=0; i < limit; i++) {
      if(predicate(this->measureAccum[i])) {
        return this->data[i]->find(combine, predicate, this->measurer->accum);
      }
    }
  } else {
    // TODO: use binary search
    std::pair<Measure, Value> result;
    bool set;
    for(int i=0; i < limit; i++) {
      accum = this->combine(this->data[i]->measure, accum);
      this->measureAccum[i] = accum;
      if(predicate(this->measureAccum[i] && !set)) {
        set = true;
        result = this->data[i]->find(combine, predicate, this->measurer->accum);
      }
    }
    return result;
  }
}

template <typename Bitmask, typename Value>
const inline SuccinctArray<Bitmask, Value>
SuccinctArray<Bitmask, Value>::fromArray(const Value *unpacked, const int size) {
#ifdef DEBUG
  assert(sizeof(Bitmask) >= size);
#endif

  if(size == 0) {
    return SuccinctArray<Bitmask, Value, void *>(NULL, 0);
  }

  Bitmask mask = {0};
  const int single_bit = 0x1;

  for(int i=0; i < size; i++) {
    if(unpacked[i] != nullptr) {
      mask = mask | (single_bit << i);
    }
  }
  // Schema is fixed after this point

  Value *rest = new Value[__builtin_popcount(mask >> 2)];

  int lower = 0;
  for(int i=2; i < size; i++) {
    if(unpacked[i] != nullptr) {
      rest[lower] = unpacked[i];
      lower++;
    }
  }

  if(mask & 0x2) {
    const HeteroArrayTwo data = new HeteroArrayTwo(unpacked[0], unpacked[1], rest);
    return SuccinctArray<Bitmask, Value>(mask, data);
  } else if(mask & 0x1) {
    const HeteroArrayOne data = new HeteroArrayOne(unpacked[0], rest);
    return SuccinctArray<Bitmask, Value, HeteroArrayOne>(mask, data);
  } else {
    const HeteroArrayNone data = new HeteroArrayZone(rest);
    return SuccinctArray<Bitmask, Value>(mask, data);
  }
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

template <typename Bitmask, typename Value>
const SuccinctArray<Bitmask, Value> *
SuccinctArray<Bitmask, Value>::append(const SuccinctArray *other) const {
  // shift bitmask by size of schema
  const int thisSize = __builtin_popcount(this->schema);
  // round up the number of levels
  const int levels = (thisSize / 3) + (3 * !!(thisSize % 3));
  const int shiftedOther = other->schema << (levels * 3);
  // and with schema to get bitmask
  const int newMask = shiftedOther & this->schema;

  const int thatSize = __builtin_popcount(other->schema);

  Value newData[] = new Value[thisSize + thatSize];
  // memcpy prefix
  memcpy(newData, this->data, thisSize * sizeof(Value));
  // memcpy suffix
  memcpy(&newData[thisSize], other->data, thatSize * sizeof(Value));
}

// The number of references the level
// contains if it is in state "state"
constexpr int footprint(const int state) {
  // If 0, return 0
  // else return 2 if the value is even
  // and 1 otherwise
  return (state ? ((state % 2 == 0) ? 2 : 1) : 0);
}


template <typename Bitmask, typename Value>
const inline SuccinctArray<Bitmask, Value> *
SuccinctArray<Bitmask, Value>::parallelUnderflow
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

  Value *newData = new Value[totalCount];

  // The underflowing levels
  memcpy(newData, &this->data[1], numBefore * sizeof(Value));

  // The first non-underflowing level, with one element removed
  if(slopFootprint != 0) {
    newData[numBefore] = futureNonUnderflow->slop;
  }
  if(futureNonUnderflow->affix) {
    newData[numBefore + slopFootprint] = futureNonUnderflow->affix;
  }

  // The elements after the first non-underflowing level
  const int copied = numBefore + slopFootprint + affixFootprint;
  memcpy(&newData[copied], this->data[numBefore], unaffectedCount * sizeof(Value));

  const HeteroArrayThree<Value> hetero = new HeteroArrayThree(newData);

  return new SuccinctArray<Bitmask, Value, HeteroArrayThree<Value>>(futureMask, hetero);
}



template <typename Bitmask, typename Value>
const inline SuccinctArray<Bitmask, Value> *
SuccinctArray<Bitmask, Value>::parallelOverflow
(const bool left, const Value elem) const {
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
  const int withoutSpecial = this->schema & ~0x2;
  const int numBefore = __builtin_popcount(withoutSpecial & (~0x0 >> numOverflowLevels));
  const int numAfter = __builtin_popcount(unaffected);
  const int slopFootprint = (futureNonOverflow.slop != nullptr) ? 1 : 0;
  const int affixFootprint = (futureNonOverflow.affix != nullptr) ? 1 : 0;
  const int unaffectedCount = __builtin_popcount(unaffected);
  const int totalCount = numBefore + numAfter + slopFootprint + affixFootprint + unaffectedCount + 1;

  Value *newData = new Value[totalCount];

  // The overflowing levels
  memcpy(newData, this->data, numBefore * sizeof(Value));

  // The first non-overflowing level, with one element added
  if(slopFootprint != 0) {
    newData[numBefore] = futureNonOverflow.slop;
  }
  if(futureNonOverflow.affix) {
    newData[numBefore + slopFootprint] = futureNonOverflow.affix;
  }

  // The elements after the first non-overflowing level
  const int copied = 1 + numBefore + slopFootprint + affixFootprint;
  memcpy(&newData[copied], this->data[numBefore], unaffectedCount * sizeof(Value));

  HeteroArrayTwo<Value> newHeteroData = new HeteroArrayTwo<Value>(elem, this->one, newData);

  return new SuccinctArray<Bitmask, Value, HeteroArrayTwo<Value>>(futureMask, newHeteroData);
}

// Observation: we only modify head except for
// in overflow, and in that case we can append
// the non-overflowing case onwards, with a simple
// memcpy and bitshift
template <typename Bitmask, typename Value>
const inline SuccinctArray<Bitmask, Value> *
SuccinctArray<Bitmask, Value>::setHeadLevel
(const int state, const Value affix, const Value slop) const {
  const int total_size = __builtin_popcount(this->schema);
  const int upper_start = __builtin_popcount(this->schema << 3);
  const int upper_size = total_size - upper_start;
  const Bitmask levelMask = bitmaskFor<Bitmask>(state);

  const int skipOne == this->schema & 0x4 != 0 ? 1 : 0;
  const int copyOne = affix >= 3 ? 1 : 0;
  const int toCopy = total_size - skipOne + copyOne;

  Value *newVersion = new Value[toCopy];

  if(copyOne != 0) {
  	newVersion[0] = affix;
  } else {
  	head->two = affix;
  }

  for(int i=copyOne; i < toCopy; i++) {
  	newVersion[i] = this->contents->rest[i];
		newVersion[i]->inc_ref();
  }

  if(slop){
    newVersion[0] = slop;
  }
  if(affix){
    newVersion[slop != NULL ? 1 : 0] = affix;
  }
  memcpy(&newVersion[toCopy], &this->data[toSkip], upper_size * sizeof(Value));

  // Mask out lower bits
  const Bitmask withoutLower = (schema & ~0x7);
  const Bitmask newMask = withoutLower | levelMask;

  switch(state) {
    case 0: {
      return new SuccinctArray<Bitmask, Value, HeteroArray<Value>>(
        newMask, new HeteroArray<Value>);
    }
    case 1: {
      return new SuccinctArray<Bitmask, Value, HeteroArray<Value>>(
        newMask, new HeteroArray<Value>);
    }
    case 2: {
      return new SuccinctArray<Bitmask, Value, HeteroArray<Value>>(
        newMask, new HeteroArray<Value>);
    }
    case 3: {
      return new SuccinctArray<Bitmask, Value, HeteroArray<Value>>(
        newMask, new HeteroArray<Value>);
    }
    case 4: {
      return new SuccinctArray<Bitmask, Value, HeteroArray<Value>>(
        newMask, new HeteroArray<Value>);
    }
  }
}

template <typename Bitmask, typename Value>
const int
SuccinctArray<Bitmask, Value>::getState(int level) const {
  // Shift out lower levels, mask to move current level to head
  const Bitmask base = (schema >> (level * 3)) & 0x7;
  const int three_affix = (base & (0x1 << 2)) ? 3 : 0;
  const int one_affix = (base & (0x1 << 1))  ? 1 : 0;
  const int slop = (base & 0x1) ? 1 : 0;
  return slop + one_affix + three_affix;
}

template <typename Bitmask, typename Value>
inline const Level<Bitmask, Value>
SuccinctArray<Bitmask, Value>::getLevel(int level, int state) const {
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
