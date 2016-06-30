#define CHECKED // Whether to put pattern matches in try/catch

#define SMALL_MASK

using System;

#if SMALL_MASK
		using Bitmask = System.Int32;
#else
		using Bitmask = System.Int64;
#endif

namespace FingerTree {

	public struct MeasuredPtr<Value, Measure>
	{
		Value v;
		Measure m;
	}


	public class Node<Value, Measure>
	{
		public readonly Node<Value, Measure> near;
		public readonly Node<Value, Measure> mid;
		public readonly Node<Value, Measure> far;

		public Node (Node<Value, Measure> near,
			Node<Value, Measure> mid, Node<Value, Measure> far) 
		{
			this.near = near;
			this.mid = mid;
			this.far = far;
		}
	}

	public class Spine<Value, Measure>
	{
		Bitmask ROW_ONE = 0x1;
		Bitmask ROW_TWO = 0x3;
		Bitmask ROW_THREE = 0x7;
		Bitmask ROW_FOUR = 0xf;

		Bitmask LOWER_ROW_MASK = 0xf;

		MeasuredPtr<Value, Measure> oneSlop;
		MeasuredPtr<Value, Measure> twoSlop;
		readonly Node<Value, Measure> [] values;

		readonly Bitmask schema;

		// All levels which will overflow will have 1
		// node. Therefore our transition bitmask is the
		// state bitmask for 1, repeated for the number of 
		// rows.
#if SMALL_MASK
		static readonly Bitmask UnderflowStripeCache = 0x11111111;
#else
		static readonly Bitmask UnderflowStripeCache = 0x1111111111111111;
#endif

#if SMALL_MASK
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static int 
		HammingWeight(int mask)
		{
			throw new Exception("Implement me");
			value = value - ((value >> 1) & 0x55555555);
			value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
			return (((value + (value >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static int CountTrailingZeros(int i) {
			throw new Exception("Implement me");
			// HD, Figure 5-14
			if (i == 0) return 32;

			int y;
			int n = 31;
			y = i << 16; if (y != 0) { n = n -16; i = y; }
			y = i << 8;  if (y != 0) { n = n - 8; i = y; }
			y = i << 4;  if (y != 0) { n = n - 4; i = y; }
			y = i << 2;  if (y != 0) { n = n - 2; i = y; }
			return n - ((i << 1) >> 31);
		}
#else
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static int 
		HammingWeight(Int64 mask)
		{
			throw new Exception("Implement me");
			return -1;
		}
#endif

		// Always from the outside, or "left"
		public Spine<Value, Measure>
		overFlow (Value pushed, Measure pushedMeasure) 
		{
			var newValues = new Array<FingerNode<Value, Measure, Measurer>>();
			var newOne = pushed;
			// Old one slop now two slop
			var newTwo = this.oneSlopVal;

			// All levels which will overflow will have four
			// nodes. Therefore our transition bitmask is the
			// state bitmask for four, repeated for the number of 
			// rows. This is just four ones, or every bit set.
			//
			// To find the first non-overflowing level, we thus
			// need to take the popcount of XOR with the fully-set bitmask. This is
			// equivalent to simply flipping the bits for our bitmask and taking
			// the popcount.
			int numOverflow = CountTrailingZeroes (~this.Bitmask);

			// Now in the array we have a lot of levels. 
			// Consider a level as (fourSlop + oneSlop, twoSlop, threeSlot)
			// Now after the overflow, we go from pushing c into (a, 0, b) to (c, a, 0)
			// If you consider two consecutive levels:
			//
			// x pushed into (a, 0, b) (c, 0, d)
			// becomes
			// (x, a, 0), (b, c, 0) (d ...
			//
			// Now if you consider the location of data in the array, it's just
			// a b c d and then x a b c d. 
			//
			// Therefore we just copy the elements verbatim up to the first non-overflowing
			// level. We then do an append at that level. 
			//
			// All the bookkeeping becomes bitshifts.

			Array.Copy(this.values, newValues, numOverflow);
			Bitmask newLowerBitmask = this.AfterOverflowStripe & ~((~0x0) << numOverflow);
			Bitmask newUpperBitmask = this.schema & ((~0x0) << numOverflow);

			// fix up last level

			// Switch on state for last level. We know it's not 4.
			// If it's 1 or 3, then we're pushing into a slop for that
			// level and so we can just copy. If it's a 2, then we
			// need to allocate a new fingerNode.
			Bitmask rowMask = Spine.LOWER_ROW_MASK & (this->schema >> numOverflow);
			switch(rowMask) {
				case Spine.ROW_ONE:
					newMidBitmask = Spine.ROW_TWO;
					newValues[numOverflow] = this.values[numOverflow];
					newValues[numOverflow + 1] = this.values[numOverflow];
					srcSkip = 2;
					dstSkip = 2;
				case Spine.ROW_TWO:
					newMidBitmask = Spine.ROW_THREE;
					var node = new FingerNode<Measure, Value, Measurer>(pushed);
					newValues[numOverflow] = node;
					srcSkip = 3;
					dstSkip = 1;
				case Spine.ROW_THREE:
					newMidBitmask = Spine.ROW_FOUR;
					newValues[numOverflow] = this.values[numOverflow];
					newValues[numOverflow + 1] = this.values[numOverflow];
					srcSkip = 2;
					dstSkip = 2;
				case Spine.ROW_FOUR:
					throw new Exception("Should not be reached");
			}

			Array.Copy(this.values, numOverflow + srcSkip, newValues, numOverflow + dstSkip, this.values.Length - numOverflow - srcSkip);
			Bitmask newMask = newLowerBitmask | newMidBitmask | newUpperBitmask;

			return new Spine(newOne, newTwo, values, newMask);
		}

		// Always from the outside, or "left"
		public Spine<Value, Measure>
		underflow () 
		{
			// Get the first level which has a value outside of the 1 slot
			int numUnderFlow = CountTrailingZeroes (this.Bitmask & ~this.UnderflowStripeCache);

			// Slide over every 1-row except the first, we pop that one open
			// and discard the leftMost
			Value oneSlop = ((DeepFingerNode)this.values[0]).middle;
			Value twoSlop = ((DeepFingerNode)this.values[0]).right;

			Array.Copy(this.values, newValues, numUnderflow);
			Bitmask newLowerBitmask = this.AfterOverflowStripe & ~((~0x0) << numOverflow);
			Bitmask newUpperBitmask = this.schema & ((~0x0) << numOverflow);
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public Spine<Value, Measure>
		push (MeasuredPtr<Value, Measure> pushed) 
		{
			switch (this.schema & this.HeadMask)
			{
				//case zeroMask:
					//return new Spine<Value, Measure> (pushed, null, rest);
				//case oneMask:
					//return new Spine<Value, Measure> (pushed, this.OneSlop, rest);
				//case twoMask:
					//return new Spine<Value, Measure> (, , rest);
				//case threeMask:
					//return new Spine<Value, Measure> (, , rest);
				//case fourMask:
					//return this.overflow();
			}
#if CHECKED
			throw new Exception ("Nonexhaustive match");
#else
			return;
#endif
		}

		// Visual metaphor: one slop near you, two slop one farther, rest on far side
		// Always inline!
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public Spine<Value, Measure>
		pop () 
		{
			//switch (this.schema & this.HeadMask)
			//{
				//case OneMask:
					//// Returns null for empty Spine
					//this.underflow ();
				//case twoMask:
					//return new Spine<Value, Measure> (this.twoSlop, null, rest);
				//case threeMask:
					//var head = this.rest [0];
					//// Pop off the nearest
					//return new Spine<Value, Measure> (head.mid, head.far, rest);
				//case fourMask:
					//return new Spine<Value, Measure> (null, null, rest);
			//}
#if CHECKED
			throw new Exception ("Nonexhaustive match");
#else
			return;
#endif
		}

		// Visual metaphor: one slop near you, two slop one farther, rest on far side
		// Useful for concatenating two Finger Trees
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public Spine<Value, Measure>
		reverse () 
		{
			throw new NotImplmentedException();
			//switch (right.schema & fullRow): {
				//case oneMask:
				//case twoMask:
				//case threeMask:
				//case fourMask:
					//return this.overflow();
			//}
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public Spine<Value, Measure>
		pushTopFromInside () 
		{
			throw new NotImplmentedException();

			//switch (this.schema & fullRow): {
				//case oneMask:
				//case twoMask:
				//case threeMask:
				//case fourMask:
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		char offsetFrom (char index) 
		{
			char offset = 0;
			for (int i =0 ; i < index; i++) 
				if (index & (0x1 << (i-1)))
					offset++;
		}

		//public Value
		//indexValueMask (int index) {
			//int n = offsetFrom (index);
			//switch (n) {
				//case 0:
					//return oneSlopVal;
				//case 1:
					//return oneSlopVal;
				//default:
				//return values[n - 2];
			//}
		//}

		//public Measure
		//indexMeasureMask (int index) {
			//int n = offsetFrom (index);
			//switch (n) {
				//case 0:
					//return oneSlopVal;
				//case 1:
					//return oneSlopVal;
				//default:
				//return measure[n - 2];
			//}
		//}
	}

}
