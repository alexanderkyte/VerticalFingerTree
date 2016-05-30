#ifndef __FINGER_TREE_SUCC_ARRY
#define __FINGER_TREE_SUCC_ARRY

#include <assert.h>
#include <cstring>
#include "level.hpp"

// Bitmask size calculuation guide
//
// Each level at depth n has at most 3 * 3^{n} + 3^{n + 1} nodes. 
// This simplifies to 2 * 3^{n + 1} nodes. 
// The summation of the levels up to depth n is 3^{m + 2} - 3.
// That implies that if you have 4m bits, then you can hold 
// 3^{m + 2} - 3 items. 

template <typename Value, typename Measure>
struct HeteroArrayAny {
};

template <typename Value, typename Measure>
struct HeteroArrayRest: HeteroArrayAny<Value, Measure> {
  typedef MeasuredPtr<std::shared_ptr<FingerNode<Value, Measure>>, Measure> DeeperNode;
  const std::vector<DeeperNode> rest;

  HeteroArrayRest(std::vector<DeeperNode> rest): rest(rest) {}
};

template <typename Value, typename Measure>
struct HeteroArrayOne: HeteroArrayRest<Value, Measure> {
  typedef MeasuredPtr<std::shared_ptr<FingerNode<Value, Measure>>, Measure> DeeperNode;
  const MeasuredPtr<Measure, Value> slop;

  HeteroArrayOne(const MeasuredPtr<Value, Measure> one, const std::vector<DeeperNode> rest):
    slop(one), HeteroArrayRest<Value, Measure>(rest) {}
};

template <typename Value, typename Measure>
struct HeteroArrayTwo: HeteroArrayRest<Value, Measure> {
  typedef MeasuredPtr<std::shared_ptr<FingerNode<Value, Measure>>, Measure> DeeperNode;

  const MeasuredPtr<Measure, Value> slop;
  const MeasuredPtr<Measure, Value> affix;

  HeteroArrayTwo(const MeasuredPtr<Value, Measure> one, const MeasuredPtr<Value, Measure> two, const std::vector<DeeperNode> rest):
    slop(one), affix(two), HeteroArrayRest<Value, Measure>(rest) {}
};

template <typename Value, typename Measure>
struct HeteroArrayThree: public HeteroArrayRest<Value, Measure> {
  typedef MeasuredPtr<std::shared_ptr<FingerNode<Value, Measure>>, Measure> DeeperNode;

  HeteroArrayThree(std::vector<DeeperNode> rest): HeteroArrayRest<Value, Measure>(rest) {}
};

template <typename Value, typename Measure>
struct HeteroArrayFour: public HeteroArrayOne<Value, Measure> {
  typedef MeasuredPtr<std::shared_ptr<FingerNode<Value, Measure>>, Measure> DeeperNode;

  HeteroArrayFour(const MeasuredPtr<Value, Measure> slop, const std::vector<DeeperNode> rest):
    HeteroArrayOne<Value, Measure>(slop, rest) {}
};


template <typename Bitmask, typename Value, typename ArrayType, typename Measure>
class SuccinctArray {
  typedef Level<Bitmask, Value, Measure> LevelType;

  private:
    // This contains an array prefix
    // that has unboxed references to values
    const ArrayType contents;
    const Bitmask schema;

    SuccinctArray(Bitmask mask, ArrayType *contents): contents(*contents), schema(mask) {}

  public:
    SuccinctArray(void): contents(), schema() {}

    const inline SuccinctArray
    append(const SuccinctArray *other) const;

    const inline SuccinctArray
    setHeadLevel(const int, const Value affix, const Value slop) const;

    const inline LevelType
    getLevel(int level, int state) const;

    const inline int
    getState(int level) const;

    const inline SuccinctArray
    offset(int level) const;

    //static
    //const inline SuccinctArray
    //fromArray(const Value *unpacked, const int size);

    const inline SuccinctArray
    parallelUnderflow
    (const bool left) const;

    const inline SuccinctArray
    parallelOverflow
    (const bool left, const Value elem) const;

    const inline FingerNode<Value, Measure>
    find(const Measure *goal, const Measure *accum) const;

    const inline FingerNode<Value, Measure>
    findInner 
    (const FingerNode<Value, Measure> node, const Measure accum, const Measure goal, const int level) const;
};

template <typename Bitmask, typename Value, typename ArrayType, typename Measure>
const inline FingerNode<Value, Measure>
SuccinctArray<Bitmask, Value, ArrayType, Measure>::findInner 
(const FingerNode<Value, Measure> node, const Measure accum, const Measure goal, const int level) const 
{
    Measure prev = accum;
    FingerNode<Value, Measure> cursor = node;

    // We have a precondition that the answer is in the sub-tree

    for (int i = 0; i < level; i++) {
        accum = accum.combine(cursor [0]);
        if (accum >= goal) {
            cursor = cursor [0];
            continue;
        } else {
            prev = accum;
        }
        accum = accum.combine(cursor [1]);
        if (accum >= goal) {
            cursor = cursor [1];
            continue;
        } else {
            prev = accum;
        }
        accum = accum.combine(cursor [2]);
        if (accum >= goal) {
            cursor = cursor [2];
            continue;
        } else {
            throw new std::exception("Shouldn't reach this. This implies that a node was searched when we didn't know an answer was internal.");
        }
    }
    // We should have reached the last level, and so should
    // have found the right leaf. 

    return cursor
}

// Find
template <typename Bitmask, typename Value, typename ArrayType, typename Measure>
const inline FingerNode<Value, Measure>
SuccinctArray<Bitmask, Value, ArrayType, Measure>::find(const Measure goal) const {
    Measure accum = Measurer.getIdentity();
    Measure prev = accum;
    if(this->schema & 0x1
        accum = accum.combine(this->oneSlop.Measure);

    if(accum >= goal
        return findInner(this->oneSlop, prev, goal);
    
    if(this->schema & 0x2)
        prev = accum;
        accum = accum.combine(this->oneSlop.Measure);

    if(accum >= goal)
        return findInner(this->twoSlop, prev, goal);

    const int limit = __builtin_popcount(this->schema >> 2);

    for(int i=0; i < limit; i++) {
        prev = goal;
        accum = accum->combine(this->contents[i]);
        if(goal <= accum) {
            return findInner(this->contents[i], prev);
        } 
    }
}

//template <typename Bitmask, typename Value>
//const inline SuccinctArray<Bitmask, Value>
//SuccinctArray<Bitmask, Value>::fromArray(const Value *unpacked, const int size) {
//#ifdef DEBUG
  //assert(sizeof(Bitmask) >= size);
//#endif

  //if(size == 0) {
    //return SuccinctArray<Bitmask, Value, void *>(NULL, 0);
  //}

  //Bitmask mask = {0};
  //const int single_bit = 0x1;

  //for(int i=0; i < size; i++) {
    //if(unpacked[i] != nullptr) {
      //mask = mask | (single_bit << i);
    //}
  //}
  //// Schema is fixed after this point

  //Value *rest = new Value[__builtin_popcount(mask) >> 2)];

  //int lower = 0;
  //for(int i=2; i < size; i++) {
    //if(unpacked[i] != nullptr) {
      //rest[lower] = unpacked[i];
      //lower++;
    //}
  //}

  //if(mask & 0x2) {
    //const HeteroArrayTwo contents = new HeteroArrayTwo(unpacked[0], unpacked[1], rest);
    //return SuccinctArray<Bitmask, Value>(mask, contents);
  //} else if(mask & 0x1) {
    //const HeteroArrayOne contents = new HeteroArrayOne(unpacked[0], rest);
    //return SuccinctArray<Bitmask, Value, HeteroArrayOne>(mask, contents);
  //} else {
    //const HeteroArrayNone contents = new HeteroArrayZone(rest);
    //return SuccinctArray<Bitmask, Value>(mask, contents);
  //}
//}

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

template <typename Bitmask, typename Value, typename ArrayType, typename Measure>
const inline SuccinctArray<Bitmask, Value, ArrayType, Measure>
SuccinctArray<Bitmask, Value, ArrayType, Measure>::append(const SuccinctArray *other) const {
  // shift bitmask by size of schema
  const int thisSize = __builtin_popcount(this->schema);
  // round up the number of levels
  const int levels = (thisSize / 3) + (3 * !!(thisSize % 3));
  const int shiftedOther = other->schema << (levels * 3);
  // and with schema to get bitmask
  const int newMask = shiftedOther & this->schema;

  const int thatSize = __builtin_popcount(other->schema);

  Value newcontents[] = new Value[thisSize + thatSize];
  // memcpy prefix
  memcpy(newcontents, this->contents, thisSize * sizeof(Value));
  // memcpy suffix
  memcpy(&newcontents[thisSize], other->contents, thatSize * sizeof(Value));
}

// The number of references the level
// contains if it is in state "state"
constexpr int footprint(const int state) {
  // If 0, return 0
  // else return 2 if the value is even
  // and 1 otherwise
  return (state ? ((state % 2 == 0) ? 2 : 1) : 0);
}


template <typename Bitmask, typename Value, typename ArrayType, typename Measure>
const inline SuccinctArray<Bitmask, Value, ArrayType, Measure>
SuccinctArray<Bitmask, Value, ArrayType, Measure>::parallelUnderflow
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

  // Note: We pop one element from the front of the contents
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

  Value *newcontents = new Value[totalCount];

  // The underflowing levels
  memcpy(newcontents, &this->contents[1], numBefore * sizeof(Value));

  // The first non-underflowing level, with one element removed
  if(slopFootprint != 0) {
    newcontents[numBefore] = futureNonUnderflow->slop;
  }
  if(futureNonUnderflow->affix) {
    newcontents[numBefore + slopFootprint] = futureNonUnderflow->affix;
  }

  // The elements after the first non-underflowing level
  const int copied = numBefore + slopFootprint + affixFootprint;
  memcpy(&newcontents[copied], this->contents[numBefore], unaffectedCount * sizeof(Value));

  const auto hetero = new HeteroArrayThree<Value, Measure>(newcontents);

  return new SuccinctArray<Bitmask, Value, HeteroArrayThree<Value, Measure>, Measure>(futureMask, hetero);
}



template <typename Bitmask, typename Value, typename ArrayType, typename Measure>
const inline SuccinctArray<Bitmask, Value, ArrayType, Measure>
SuccinctArray<Bitmask, Value, ArrayType, Measure>::parallelOverflow
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

  Value *newcontents = new Value[totalCount];

  // The overflowing levels
  memcpy(newcontents, this->contents, numBefore * sizeof(Value));

  // The first non-overflowing level, with one element added
  if(slopFootprint != 0) {
    newcontents[numBefore] = futureNonOverflow.slop;
  }
  if(futureNonOverflow.affix) {
    newcontents[numBefore + slopFootprint] = futureNonOverflow.affix;
  }

  // The elements after the first non-overflowing level
  const int copied = 1 + numBefore + slopFootprint + affixFootprint;
  memcpy(&newcontents[copied], this->contents[numBefore], unaffectedCount * sizeof(Value));

  const auto newHeterocontents = new HeteroArrayTwo<Value, Measure>(elem, this->one, newcontents);

  return new SuccinctArray<Bitmask, Value, HeteroArrayTwo<Value, Measure>, Measure>(futureMask, newHeterocontents);
}

// Observation: we only modify head except for
// in overflow, and in that case we can append
// the non-overflowing case onwards, with a simple
// memcpy and bitshift
template <typename Bitmask, typename Value, typename ArrayType, typename Measure>
const inline SuccinctArray<Bitmask, Value, ArrayType, Measure>
SuccinctArray<Bitmask, Value, ArrayType, Measure>::setHeadLevel
(const int state, const Value affix, const Value slop) const {
  const int total_size = __builtin_popcount(this->schema);
  const int upper_count = __builtin_popcount(this->schema >> 2);

  const int skipOne = (this->schema & 0x4) != 0 ? 1 : 0;
  const int copyOne = (this->state >= 3) ? 1 : 0;

  std::vector<std::shared_ptr<FingerNode<Value, Measure>>> newVersion = {0};

  if(copyOne + skipOne == 0) {
    newVersion = this->contents.rest;
  } else {
    newVersion = new std::vector<std::shared_ptr<FingerNode<Value, Measure>>>(upper_count + copyOne);

    if(copyOne != 0) {
      newVersion[0] = affix;
    }

    for(int i=0; i < upper_count; i++) {
      newVersion[copyOne + i] = this->contents->rest[skipOne + i];
    }
  }

  // Mask out lower bits
  const Bitmask withoutLower = (schema & ~0x7);
  const Bitmask levelMask = bitmaskFor<Bitmask>(state);
  const Bitmask newMask = withoutLower | levelMask;

  switch(state) {
    case 0: {
      assert(0 && "Not reached");
    }
    case 1: {
      return new SuccinctArray<Bitmask, Value, HeteroArrayOne<Value, Measure>, Measure>(
        newMask, new HeteroArrayOne<Value, Measure>(slop, newVersion));
    }
    case 2: {
      return new SuccinctArray<Bitmask, Value, HeteroArrayOne<Value, Measure>, Measure>(
        newMask, new HeteroArrayTwo<Value, Measure>(slop, affix, newVersion));
    }
    case 3: {
      return new SuccinctArray<Bitmask, Value, HeteroArrayThree<Value, Measure>, Measure>(
        newMask, new HeteroArrayThree<Value, Measure>(newVersion));
    }
    case 4: {
      return new SuccinctArray<Bitmask, Value, HeteroArrayFour<Value, Measure>, Measure>(
        newMask, new HeteroArrayFour<Value, Measure>(slop, newVersion));
    }
  }
}

template <typename Bitmask, typename Value, typename ArrayType, typename Measure>
const int
SuccinctArray<Bitmask, Value, ArrayType, Measure>::getState(int level) const {
  // Shift out lower levels, mask to move current level to head
  const Bitmask base = (schema >> (level * 3)) & 0x7;
  const int three_affix = (base & (0x1 << 2)) ? 3 : 0;
  const int one_affix = (base & (0x1 << 1))  ? 1 : 0;
  const int slop = (base & 0x1) ? 1 : 0;
  return slop + one_affix + three_affix;
}

template <typename Bitmask, typename Value, typename ArrayType, typename Measure>
inline const Level<Bitmask, Value, Measure>
SuccinctArray<Bitmask, Value, ArrayType, Measure>::getLevel(int level, int state) const {
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
      return LevelType(state, nullptr, this->contents[0 + offset]);
    case 2:
    case 4:
      return LevelType(state, this->contents[1 + offset], this->contents[0 + offset]);
    case 3:
      return LevelType(state, this->contents[0 + offset], nullptr);
  }

  assert(0 && "Not reached");
  return LevelType(state, nullptr, nullptr);
}

#endif
