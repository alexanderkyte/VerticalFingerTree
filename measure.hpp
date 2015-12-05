#ifndef __FINGER_TREE_MEASURE
#define __FINGER_TREE_MEASURE

template <typename Value>
class Measure {
public:
  const inline bool
  greaterThan(Measure<Value> other) = {0};

  const inline Measure
  combine(Measure left, Measure right) = {0};

  const inline Measure
  measure(Value item) = {0};

  const inline Measure 
  getidentity(void) = {0};
}

#endif
