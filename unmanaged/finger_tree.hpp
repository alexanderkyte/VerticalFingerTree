#ifndef __FINGER_TREE_H
#define __FINGER_TREE_H

#include "succ_array.hpp"
#include "level.hpp"
#include "measure.hpp"

template <typename Bitmask, typename Measure, typename Value, typename Measurer>
class FingerTree {
  // TODO: specialize this, to get an inlined level setup
  typedef SuccinctArray<Bitmask, Value, HeteroArrayAny<Value, Measure>, Measure> Spine;
  typedef Level<Bitmask, Value, Measure> LevelType;

  private:
    const Spine left;
    const Spine right;

    FingerTree(const Spine *left, const Spine *right):
      left(*left), right(*right) {}

    const inline FingerTree<Bitmask, Measure, Value, Measurer> *
    push(const bool left, const Spine *near,
         const Spine *far, const Value elem) const;

    const inline FingerTree<Bitmask, Measure, Value, Measurer> *
    pop(const bool left, const Spine *near, const Spine *far) const;

    FingerTree(bool left, const Spine *near, const Spine *far) {
      if(left) {
        FingerTree(near, far);
      } else {
        FingerTree(far, near);
      }
    }

  public:
    FingerTree(void): left(), right() {}

    const FingerTree<Bitmask, Measure, Value, Measurer> *
    pushRight(const Value elem) const {
      return push(true, &this->right, &this->left, elem);
    }

    const FingerTree<Bitmask, Measure, Value, Measurer> *
    popRight(void) const {
      return pop(true, &this->right, &this->left);
    }

    const FingerTree<Bitmask, Measure, Value, Measurer> *
    pushLeft(const Value elem) const {
      return push(true, &this->left, &this->right, elem);
    }

    const FingerTree<Bitmask, Measure, Value, Measurer> *
    popLeft(void) const {
      return pop(true, &this->left, &this->right);
    }

    const Value 
    peekRight(void) const;

    const Value 
    peekLeft(void) const;

    const std::tuple<bool, Value>
    find(const Measure goal) const {
      Measure start = Measurer::getIdentity();
      auto leftRes = left.find(goal, start);

      if(leftRes.value != (Value){0}) {
        return std::tuple<bool, Value>(true, leftRes.value);
      }

      auto rightRes = right.find(&leftRes);

      if(rightRes.predicate(goal)) {
        return std::tuple<bool, Value>(true, rightRes.value);
      } else {
        return std::tuple<bool, Value>(false, (Value){0});
      }
    }
};

//
// Push
//
template <typename Bitmask, typename Measure, typename Value, typename Measurer>
const inline FingerTree<Bitmask, Measure, Value, Measurer> *
FingerTree<Bitmask, Measure, Value, Measurer>::push(const bool left, const Spine *near, const Spine *far, const Value elem) const
{
  const int near_state = near->getState(0);
  const int far_state = far->getState(0);

  if(near_state == 0 && far_state < 2) {
    const LevelType curr = far->getLevel(0, far_state);
    const LevelType next = curr.add(left, elem);
    const Spine *newFar = 
      near->setHeadLevel(far_state + 1, next.affix, next.slop);

    return new FingerTree(left, near, newFar);
  } else {
    if(near_state == 4) {
        const Spine *newNear = near->parallelOverflow(left, elem);
        return new FingerTree(left, newNear, far);
    } else {
        const LevelType curr = near->getLevel(0, near_state);
        const LevelType next = curr.add(left, elem);
        const Spine *newNear = near->setHeadLevel(near_state + 1, next.affix, next.slop);
        return new FingerTree(left, newNear, far);
    }
  }
}

//
// Pop
//
template <typename Bitmask, typename Measure, typename Value, typename Measurer>
const inline FingerTree<Bitmask, Measure, Value, Measurer> *
FingerTree<Bitmask, Measure, Value, Measurer>::pop(const bool left, const Spine *near, const Spine *far) const
{
  const int near_state = near->getState(0);
  const int far_state = far->getState(0);

  if(near_state == 0) {
    const LevelType curr = far->getLevel(0, far_state);
    const LevelType next = curr.remove(left);
    const auto newFar = far->setHeadLevel(far_state + 1, next);
    return new FingerTree(left, near, newFar);
  } else {
    if(near_state == 1) {
      const Spine newNear = near->parallelUnderflow(left);
      return new FingerTree(left, newNear, far);
    } else {
      const LevelType curr = near->getLevel(0, near_state);
      const LevelType next = curr.remove(left);
      const Spine newNear = near->setHeadLevel(near_state + 1, next);
      return new FingerTree(left, newNear, far);
    }
  }
}

//
// Peek
//

template <typename Bitmask, typename Measure, typename Value, typename Measurer>
const inline Value
FingerTree<Bitmask, Measure, Value, Measurer>::peekLeft(void) const 
{
  const int left_state = this->left.getState(0);
  const int right_state = this->right.getState(0);

  if(left_state == 0) {
    LevelType curr = right.getLevel(0, right_state);
    switch(right_state) {
      case 0:
        return {0};
      case 1:
        return this->right.contents->slop.value;
      case 2:
        return this->right.contents->affix.value;
      default:
        assert(0 && "Should not be reached");
        return {0};
    }
  } else {
    LevelType curr = left.getLevel(0, left_state);
    switch(left_state) {
      case 0:
        return {0};
      case 4: 
      case 1:
      case 2:
        return this->left.contents->slop.value;
      case 3:
        return ((FingerNode<Measure, Value> *)this->left.contents->rest[0])->value;
      default:
        assert(0 && "Should not be reached");
        return {0};
    }
  }
}

template <typename Bitmask, typename Measure, typename Value, typename Measurer>
const inline Value
FingerTree<Bitmask, Measure, Value, Measurer>::peekRight(void) const 
{
  const int left_state = this->left.getState(0);
  const int right_state = this->right.getState(0);

  if(right_state == 0) {
    switch(left_state) {
      case 0:
        return {0};
      case 1:
        return this->left.contents->slop.value;
      case 2:
        return this->left.contents->affix.value;
      default:
        assert(0 && "Should not be reached");
        return {0};
    }
  } else {
    LevelType curr = right.getLevel(0, right_state);
    switch(right_state) {
      case 0:
        return {0};
      case 4: 
      case 1:
      case 2:
        return this->right.contents->slop.value;
      case 3:
        return ((FingerNode<Measure, Value> *)this->right.contents->rest[0])->value;
      default:
        assert(0 && "Should not be reached");
        return {0};
    }
  }
}


#endif
