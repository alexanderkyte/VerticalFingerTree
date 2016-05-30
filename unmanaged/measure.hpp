#ifndef __FINGER_TREE_MEASURE
#define __FINGER_TREE_MEASURE

template <typename MeasureType, typename ValueType>
class Measurer {
public:
  const virtual inline MeasureType 
  combine(const MeasureType left, const MeasureType right) const = 0;

  const virtual inline MeasureType
  measure(ValueType item) const = 0;

  const virtual inline MeasureType
  getIdentity(void) const = 0;
};

#endif
