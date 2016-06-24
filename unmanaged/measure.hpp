#ifndef __FINGER_TREE_MEASURE
#define __FINGER_TREE_MEASURE

template <typename MeasureType, typename ValueType>
class Measurer {
public:
  const static inline MeasureType 
  combine(const MeasureType left, const MeasureType right);

  const static inline MeasureType
  measure(ValueType item);

  const static inline MeasureType
  getIdentity(void);
};

#endif
