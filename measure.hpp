#ifndef __FINGER_TREE_MEASURE
#define __FINGER_TREE_MEASURE

template <typename MeasureType, typename ValueType>
class MeasuredPtr {
public:
  const MeasureType measure_val;
  const ValueType value;

  const virtual inline bool
  predicate(const MeasuredPtr<MeasureType, ValueType> other) const = 0;

  const virtual inline MeasuredPtr<MeasureType, ValueType> 
  combine(const MeasuredPtr<MeasureType, ValueType> left,
          const MeasuredPtr<MeasureType, ValueType> right) const = 0;

  const virtual inline MeasuredPtr<MeasureType, ValueType> 
  measure(ValueType item) const = 0;

  const virtual inline MeasuredPtr<MeasureType, ValueType>
  getIdentity(void) const = 0;

  MeasuredPtr(const ValueType value) {
    this->value = value;
    this->measureVal = this->measure(value);
  }
};

#endif
