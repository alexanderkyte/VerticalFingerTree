#ifndef __FINGER_TREE_MEASURE
#define __FINGER_TREE_MEASURE

template <typename Value, typename DerivedType>
class Measure {
public:
  const virtual inline bool
  predicate(const DerivedType other) const = 0;

  const virtual inline DerivedType 
  combine(const DerivedType left, const DerivedType right) const = 0;

  const virtual inline DerivedType 
  measure(Value item) const = 0;

  const virtual inline DerivedType
  getidentity(void) const = 0;
};

#endif
