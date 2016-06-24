#ifndef __FINGER_TREE_SEQUENCE_H
#define __FINGER_TREE_SEQUENCE_H

#include "finger_tree.hpp"
#include "measure.hpp"

template <typename Value>
class SequenceLength: public Measurer<size_t, Value> {
public:
  const static inline size_t
  combine(const size_t left, const size_t right)
  {
    return left + right;
  };

  const static inline size_t
  measure(Value item)
  {
    // Every 1-list has length 1
    return 1;
  };

  const static inline size_t
  getIdentity(void)
  {
    return 0;
  };
};

// This is a random access sequence that uses finger trees
template <typename Value>
class PersistentSequence: FingerTree<long, size_t, Value, SequenceLength<Value>> {
  public:
    const Value get(size_t i) const {
      std::tuple<bool, Value> res = this->find(i);
      if(std::get<1>(res) == i) {
        return std::get<1>(res);
      } else {
        return {0};
      }
    }

    const PersistentSequence<Value> *
    append (Value v) {
    	return this->pushRight(v);
    }

};

#endif
