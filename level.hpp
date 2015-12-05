#ifndef __FINGER_TREE_LEVEL
#define __FINGER_TREE_LEVEL

#include <functional>

template <typename Type, typename Measure>
class Measurer {
  public:
    const Type identity;
    const std::function<Measure(Type)> measure;
    const std::function<Measure(Measure, Measure)> combine;

    Measurer(Type identity, std::function<Measure(Type)> measure,
      std::function<Measure(Measure, Measure)> combine):
      identity(identity), measure(measure), combine(combine) {
#ifdef DEBUG
      Measure tmp = measure(identity);
      assert(tmp == combine(tmp, tmp);
#endif
    }
}

template <typename Type, typename Measure>
class MeasuredBox { 
  public:
    const Measure measure;
    const Type value;
}


template <typename Value, typename Measure, typename Bitmask>
class FingerNode {
  std::array<Measure, 3> values;

  typedef 
  std::tuple<Measure accum, FingerTree<Bitmask> before, FingerTree<Bitmask> after>>
  Accumulator;

private:
		// Will work on find first

   //const inline
   //Accumulator
   //splitInner(const std::function<bool(Measure)> predicate, const Accumulator accum) {
     //int index = 0;
     //if(this.level == 0) {
       //Measure measureAccum = std::copy(accum->accum);
       //while(index < 3 && !predicate(measureAccum)) {
         //const Measure thisOne = measure(this->items[index]);
         //measureAccum = this->measurer->combine(measureAccum, thisOne);
         //before = before.pushRight(this->items[index]);
         //index++;
       //}
       //if(index < 3) {
         //FingerTree<Bitmask> after = FingerTree<Bitmask>.Empty();
         //while(index < 3) {
           //after = after.pushRight(this->items[index]);
         //}
         //return Accumulator(measureAccum, before, after)
       //} else {
         //return Accumulator(measureAccum, before, nullptr)
       //}
     //} else {
       //Accumulator thisCursor

       //while(index < 3 && !thisCursor->after && !predicate(measureAccum)) {
         //const Accumulator thisAccum = Accumulator(measureAccum, before, nullptr);
         //thisCursor = this->contents[index]->splitInner(predicate, accum);
         //index++;
       //}
       //if(index < 3) {
         //FingerTree<Bitmask> after
         //if(thisCursor.after) {
           //after = thisCursor.after
         //} else {
           //after = FingerTree<Bitmask>.Empty();
         //}
         //while(index < 3) {
           //after = after.pushRight(this->items[index]);
         //}
         //Accumulator nextAccum = Accumulator(measureAccum, before, after);
         //return Accumulator(measureAccum, before, after)
       //} else {
         //return Accumulator(measureAccum, before, nullptr)
       //}
     //}
   //}

  public:
   const int level;

    ~FingerNode(void) {
      if(level > 0) {
        FingerNodeInner self = (FingerNodeInner)&this;
        self.left.internal.dec_Value();
        self.right.internal.dec_Value();
        self.middle.internal.dec_Value();
      }
    }


   const inline
   std::pair<Measure, Value>
   find(const std::function<Measure(Measure, Measure)> combine,
        const std::function<bool(Measure)> predicate,
        const Measure Accum) = {0};

    FingerNode(const Value left, const Value middle, const Value right):
      left(left), middle(middle), right(right) {}

    const inline void
    dec_ref() {
      ref_count--;
      if(ref_count == 0) {
        this->~FingerNode();
      }
    }

    const inline void
    inc_ref() {
      ref_count++;
    }
}

template <typename Value>
class FingerNodeLeaf: FingerNode {
  typedef FingerNodeValue<Value> Value;
  public:
    const Value left;
    const Value middle;
    const Value right;

    const inline
    std::pair<Measure, Value>
    find(const std::function<Measure(Measure, Measure)>,  combine, const std::function<bool(Measure)> predicate, const Measure Accum) {
      Measure measureAccum = std::copy(accum->accum);
      while(index < 3 && !predicate(measureAccum)) {
        Measure thisOne = this->measures([index]);
        measureAccum = this->measurer->combine(measureAccum, thisOne);
        index++;
      }
      if(predicate(measureAccum)) {
        return std::pair<Measure, Value>(measureAccum, this->contents[i]);
      } else {
        return std::pair<Measure, Value>(measureAccum, {0});
      }
    }
};

template <typename Value>
class FingerNodeInner: FingerNode {
  typedef FingerNodeValue<Value> Value;
  public:
    const FingerNode<Value> left;
    const FingerNode<Value> middle;
    const FingerNode<Value> right;

    const inline
    std::pair<Measure, Value>
    find(const std::function<Measure(Measure, Measure)> combine, const std::function<bool(Measure)> predicate, const Measure Accum) {
      Accum localAccum = std::copy(accum);
      while(index < 3 && !predicate(localAccum::get<0>();)) {
        localAccum = findInner(combine, predicate, measureAccum);
        index++;
      }
      return localAccum;
    }
};

template <typename Bitmask, typename Value>
class Level {
  public:
    const Value affix;
    const Value slop;
    const int state;

    const inline Bitmask
    mask(void) const {
      const Bitmask slopCheck = (this->slop != nullptr) ? 0x1 : 0x0;
      const Bitmask affixOneCheck = (state == 2) ? (0x1 << 1) : 0x0;
      const Bitmask affixThreeCheck = (state >= 3) ? (0x1 << 2) : 0x0;
      return slopCheck | affixOneCheck | affixThreeCheck;
    }

    Level(const int state, const Value affix, const Value slop): 
    affix(affix), slop(slop), state(state) {}

    const inline Level<Bitmask, Value>
    add(bool left, Value elem) const {
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
            const auto node = new FingerNode<Value>(elem, this->affix, this->slop);
            return Level(3, (Value)node, nullptr);
          } else {
            const auto node = new FingerNode<Value>(this->affix, this->slop, elem);
            return Level(3, (Value)node, nullptr);
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

    const inline Level<Bitmask, Value>
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
