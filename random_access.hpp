#ifndef __FINGER_TREE_SEQUENCE_H
#define __FINGER_TREE_SEQUENCE_H

#include "finger_tree.hpp"
#include "measure"

template <typename Value, typename Internal>
class SequenceLength: Measure<Value> {
private:
  const int length;

public:
  SequenceLength(const int len): length(len);

  const inline bool
  isGreaterThanOrEqualTo(const Measure<Value> generic_other) const {
    const auto other = (SequenceLength<Value, Internal>) generic_other;
    return other->length <= this->length;
  };

  const inline int 
  combine(Measure left, Measure right) const {
    return new SequenceLength(left->length + right->length);
  };

  const inline int
  measure(Value item) {
    // Every 1-list has length 1
    return new SequenceLength(1);
  };

  const inline int
  getidentity(void) {
    return new SequenceLength(0);
  };
}

// This is a random access sequence that uses finger trees
template <typename Value>
class PersistentSequence: FingerTree<long long, int, Value> {
  public:
    // Along with pop/push for right and left we have brackets
    const Value operator [](int i) const {
      const SequenceLength thisLong = new SequenceLength(i);
      std::tuple<SequenceLength, Value> res = this->find(thisLong);
      if(res::get<0> == i) {
        return registers[i];
      } else {
        return nullptr;
      }
    }
}

#endif
