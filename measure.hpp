#ifndef __FINGER_TREE_MEASURE
#define __FINGER_TREE_MEASURE

template <typename Value>
class Measure {
public:
  const virtual inline bool
  predicate(const Measure<Value> other) const = 0;

  const virtual inline Measure
  combine(Measure left, Measure right) const = 0;

  const virtual inline Measure
  measure(Value item) const = 0;

  const virtual inline Measure 
  getidentity(void) const = 0;
}

#endif
