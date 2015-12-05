#ifndef __FINGER_TREE_BITMASK
#define __FINGER_TREE_BITMASK

//
// Start compile-time bitmask helpers
//
#define slop (0x1)
#define one_affix (0x1 << 1)
#define three_affix (0x1 << 3)

// The number of references the level
// contains if it is in state "state"
constexpr int footprint(const int state) {
  // If 0, return 0
  // else return 2 if the value is even
  // and 1 otherwise
  return (state ? ((state % 2 == 0) ? 2 : 1) : 0);
}

template <typename Bitmask>
// The bitmask for a given state
constexpr Bitmask bitmaskFor(const int state) {
  return (state == 1 ? one_affix : 0) |
    (state == 2 ? (slop | one_affix) : 0) |
    (state == 3 ? three_affix : 0) |
    (state == 4 ? (slop | three_affix) : 0);
}

template <typename Bitmask>
constexpr Bitmask
stateRepeatTimes(const Bitmask state, const int num) {
  return (state << (num * 3)) | stateRepeatTimes(state, num - 1);
}

template <typename Bitmask>
constexpr Bitmask
stateRepeat(const int state) {
  return stateRepeatTimes(bitmaskFor<Bitmask>(state), sizeof(Bitmask) / 3);
}

template <typename Bitmask>
// The bitmask for a given state
constexpr int 
state(const Bitmask bitmask) {
  return (bitmask == one_affix ? 1 : 0) |
    (bitmask == (slop | one_affix) ? 2 : 0) |
    (bitmask == three_affix ? 3 : 0) |
    (bitmask == (slop | three_affix) ? 4 : 0);
}

#undef slop
#undef one_affix
#undef three_affix
//
// End compile-time bitmask helpers
//

#endif
