#ifndef __FINGER_TREE_LEVEL
#define __FINGER_TREE_LEVEL

template <typename pointer>
class FingerNode {
  public:
    const pointer left;
    const pointer middle;
    const pointer right;

    FingerNode(const pointer left, const pointer middle, const pointer right):
      left(left), middle(middle), right(right) {};
};

template <typename Bitmask, typename pointer>
class Level {
  public:
    const pointer affix;
    const pointer slop;
    const int state;

    const inline Bitmask
    mask(void) const {
      const Bitmask slopCheck = (this->slop != nullptr) ? 0x1 : 0x0;
      const Bitmask affixOneCheck = (state == 2) ? (0x1 << 1) : 0x0;
      const Bitmask affixThreeCheck = (state >= 3) ? (0x1 << 2) : 0x0;
      return slopCheck | affixOneCheck | affixThreeCheck;
    }

    Level(const int state, const pointer affix, const pointer slop): 
    affix(affix), slop(slop), state(state) {}

    const inline Level<Bitmask, pointer>
    add(bool left, pointer elem) const {
      switch(state) {
        case 0:
          return Level(1, nullptr, elem);
        case 1:
          if(left) {
            return Level(2, elem, this->slop);
          } else {
            return Level(2, this->slop, elem);
          }
        case 2: {
          if(left) {
            const auto node = new FingerNode<pointer>(elem, this->affix, this->slop);
            return Level(3, (pointer)node, nullptr);
          } else {
            const auto node = new FingerNode<pointer>(this->affix, this->slop, elem);
            return Level(3, (pointer)node, nullptr);
          }
        }
        case 3:
          // Note: We'll never make a 3-node when pushing to
          // the far spine so this is always to the slop.
          // We do this because needlessly breaking up a 3-node
          // is wrong
          return Level(4, this->affix, elem);
        default: // case 4:
          assert(0 && "I shouldn't be reached");
      }
    }

    const inline Level<Bitmask, pointer>
    remove(bool left) const {
      switch(state) {
        case 2:
          return {.slop = this->affix};
        case 3:
          if(left) {
            return Level(this->affix.middle, this->affix.right, 2);
          } else {
            return Level(this->affix.left, this->affix.middle, 2);
          }
        case 4:
          return {.affix = this->affix};
        default: // case 0 and case 1
          assert(0 && "I shouldn't be reached");
      }
    }
};

#endif
