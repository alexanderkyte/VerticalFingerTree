#ifndef __FINGER_TREE_SEQUENCE_H
#define __FINGER_TREE_SEQUENCE_H

#include "finger_tree.hpp"
#include "measure.hpp"

template <typename Value>
class SequenceLength: public MeasuredPtr<size_t, Value> {
public:
  SequenceLength(const int len): MeasuredPtr<size_t, Value>(len) {};

  const virtual inline bool
  predicate(const MeasuredPtr<size_t, Value> *generic_other) const override {
    const auto other = (SequenceLength<Value> *) generic_other;
    return other->value <= this->value;
  };

  const virtual inline MeasuredPtr<size_t, Value>
  combine(const MeasuredPtr<size_t, Value> *left,
          const MeasuredPtr<size_t, Value> *right) const override {
    return new SequenceLength(left->value + right->value);
  };

  const virtual inline Value
  measure(Value item) const override {
    // Every 1-list has length 1
    return 1;
  };

  const virtual inline std::unique_ptr<MeasuredPtr<size_t, Value>>
  getIdentity(void) const override {
    auto ident = new SequenceLength(0);
    return new std::unique_ptr<MeasuredPtr<size_t, Value>>(ident);
  };
};

// This is a random access sequence that uses finger trees
template <typename Value>
class PersistentSequence: FingerTree<long, SequenceLength<Value>, Value> {
  public:
    const Value get(int i) const {
      const auto thisLongVal = SequenceLength<Value>(i);
      const SequenceLength<Value> *thisLong = &thisLongVal;
      std::tuple<SequenceLength<Value>, bool> res = this->find(thisLong);
      if(std::get<0>(res).value == i) {
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
