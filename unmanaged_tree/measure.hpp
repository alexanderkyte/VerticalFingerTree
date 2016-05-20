#ifndef __FINGER_TREE_MEASURE
#define __FINGER_TREE_MEASURE

template <typename MeasureType, typename ValueType>
class MeasuredPtr {
protected:
  const inline ValueType
  measureStatic(ValueType item) const {
      return measure(item);
  }

public:
  const MeasureType measure_val;
  const ValueType value;

  const virtual inline bool
  predicate(const MeasuredPtr<MeasureType, ValueType> *other) const = 0;

  const virtual inline MeasuredPtr<MeasureType, ValueType> 
  combine(const MeasuredPtr<MeasureType, ValueType> *left,
          const MeasuredPtr<MeasureType, ValueType> *right) const = 0;

  const virtual inline ValueType
  measure(ValueType item) const = 0;

  const virtual inline std::unique_ptr<MeasuredPtr<MeasureType, ValueType>>
  getIdentity(void) const = 0;

  MeasuredPtr(const ValueType value):
      MeasuredPtr(value, measureStatic(value))
      { }

  MeasuredPtr(const MeasureType measure, const ValueType value):
      value(value), measure_val(measure)
      { }
};

#endif
