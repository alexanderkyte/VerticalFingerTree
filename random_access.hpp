#ifndef __FINGER_TREE_SEQUENCE_H
#define __FINGER_TREE_SEQUENCE_H

#include "finger_tree.hpp"
#include "measure.hpp"

template <typename Value>
class SequenceLength: MeasuredPtr<size_t, Value> {
private:
  const size_t length;

public:
  SequenceLength(const int len): length(len) {};

  const virtual inline bool
  predicate(const MeasuredPtr<size_t, Value> generic_other) const override {
    const auto other = (SequenceLength<Value>) generic_other;
    return other->length <= this->length;
  };

  const virtual inline MeasuredPtr<size_t, Value>
  combine(MeasuredPtr<size_t, Value> left, MeasuredPtr<size_t, Value> right) const override {
    return new SequenceLength(left->length + right->length);
  };

  const virtual inline MeasuredPtr<MeasureType, ValueType>
  measure(Value item) const override {
    // Every 1-list has length 1
    return new SequenceLength(1);
  };

  const virtual inline SequenceLength<Value>
  getIdentity(void) const override {
    return new SequenceLength(0);
  };
};

// This is a random access sequence that uses finger trees
template <typename Value>
class PersistentSequence: FingerTree<long, SequenceLength<Value>, Value> {
  public:
    const Value get(int i) const {
      SequenceLength<Value> const thisLong = SequenceLength<Value>(i);
      std::tuple<SequenceLength<Value>, bool> res = this->find(*thisLong);
      if(std::get<0>(res) == i) {
        return std::get<1>(res);
      } else {
        return {0};
      }
    }

    const PersistentSequence<Value> append (Value v) {
    	return this->pushRight(v);
    }

};

#endif
