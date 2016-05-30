
#define FINGER_TREE_32_BIT

namespace FingerTree {
	public class SizedTree<Value, Measure, Bitmask : Numeric, TwiceBitmask> extends Tree
	{
		readonly Spine<Value, Measure> left;
		readonly Spine<Value, Measure> right;

		FingerTree<Measure, Value>
		query (Measure upper_bound)
		{
			val leftResult = left.query (upper_bound);
			if (leftResult != null)
				return leftResult;

			return right.query (upper_bound);
		}

		FingerTree<Measure, Value>
		query (Func<Measure, bool> predicate) 
		{
			val leftResult = left.query (predicate);
			if (leftResult != null)
				return leftResult;

			return right.query (predicate);
		}

		const char fullRow = 0xF;
		const char emptyRow = 0xF;

		public SizedTree<Value, Measure, Bitmask : Numeric> 
		pushLeft (Value n)
		{
			if ( (left.schema | fullRow) == emptyRow) 
			{
				right.pushLeft (n);
			}
			else
			{
				left.pushLeft (n);
			}
		}

		public SizedTree<Value, Measure, Bitmask : Numeric> 
		pushRight (Value n)
		{
			Measure measured = this.Measurer.eval (n);

			if ( (right.schema | fullRow) == emptyRow) 
			{
				left.pushRight (n, measured);
			}
			else
			{
				right.pushRight (n, measured);
			}
		}

		public SizedTree<Value, Measure, Bitmask : Numeric> 
		popLeft ()
		{
			if ( (left.schema | fullRow) == emptyRow) 
			{
				right.popLeft ();
			}
			else
			{
				left.popLeft ();
			}
		}

		public SizedTree<Value, Measure, Bitmask : Numeric> 
		popRight ()
		{
			if ( (right.schema | fullRow) == emptyRow) 
			{
				left.popRight ();
			}
			else
			{
				right.popRight ();
			}
		}

		public SizedTree<Value, Measure, Bitmask : Numeric> 
		peekLeft ()
		{
			if ( (left.schema | fullRow) == emptyRow) 
			{
				right.peekLeft ();
			}
			else
			{
				left.peekLeft ();
			}
		}

		public SizedTree<Value, Measure, Bitmask : Numeric> 
		peekRight ()
		{
			if ( (right.schema | fullRow) == emptyRow) 
			{
				left.peekRight ();
			}
			else
			{
				right.peekRight ();
			}
		}

	}

	// Make a "Multimeasurer example"
	// This just has a tuple for values,
	// and different predicates are used
	// ex: indexed priority queue

	public interface Measurer<Value, Measure>
	{
		public Measure
		combine (Measure left, Measure right);

		public Measure
		eval (Value val);

		public Measure
		smallest ();
	}

	public class Spine<Value, Measure, Bitmask>
	{

		Value oneSlopVal;
		Measure oneSlopMeasure
		Value TwoSlopVal;
		Measure TwoSlopMeasure

		readonly Array<FingerNode<Measures>> measures
		readonly Array<FingerNodex<Value>> values
		readonly Bitmask schema;

		// All levels which will overflow will have 1
		// node. Therefore our transition bitmask is the
		// state bitmask for 1, repeated for the number of 
		// rows.
#if FINGER_TREE_32_BIT
		static readonly Bitmask UnderflowStripeCache = 0x11111111;
#else
		static readonly Bitmask UnderflowStripeCache = 0x1111111111111111;
#endif

#if FINGER_TREE_32_BIT
		public static int 
		HammingWeight(int mask)
		{
			value = value - ((value >> 1) & 0x55555555);
			value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
			return (((value + (value >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
		}
#else
		public static int 
		HammingWeight(int64 mask)
		{
			throw new Exception("Implement me");
			return -1;
		}
#endif

		static Spine ()
		{
			Bitmask zeroes = 0x0;
			this.OverflowStripeCache = ~ zeroes;

			this.UnderflowStripeCache = ~ zeroes;
		}

		public Spine<Value, Measure>
		overFlow (Value, Measure) 
		{
			// All levels which will overflow will have four
			// nodes. Therefore our transition bitmask is the
			// state bitmask for four, repeated for the number of 
			// rows. This is just four ones, or every bit set.
			//
			// To find the first non-overflowing level, we thus
			// need to take the popcount of XOR with the fully-set bitmask. This is
			// equivalent to simply flipping the bits for our bitmask and taking
			// the popcount.
			int numOverflow = HammingWeight(~this.Bitmask);
		}

		public Spine<Value, Measure>
		underflow (Value, Measure) 
		{
		
		}

		FingerTree<Measure, Value>
		query (Measure upper_bound)
		{
		}

		FingerTree<Measure, Value>
		query (Func<Measure, bool> predicate) 
		{
		}

		public Spine<Value, Measure>
		popLeft (Value, Measure)
		{
			switch (right.schema & fullRow): {
				case oneMask:
				case twoMask:
				case threeMask:
				case fourMask:
			}
		}

		public Spine<Value, Measure>
		pushLeft (Value, Measure)
		{
			switch (right.schema & fullRow): {
				case oneMask:
				case twoMask:
				case threeMask:
				case fourMask:
			}
		}

		public Spine<Value, Measure>
		popRight (Value, Measure)
		{
			switch (right.schema & fullRow): {
				case oneMask:
				case twoMask:
				case threeMask:
				case fourMask:
			}
		}

		public Spine<Value, Measure>
		pushRight (Value, Measure)
		{
			switch (right.schema & fullRow): {
				case oneMask:
				case twoMask:
				case threeMask:
				case fourMask:
			}
		}

		char offsetFrom (char index) 
		{
			char offset = 0;
			for (int i =0 ; i < index; i++) 
				if index & (0x1 << (i-1))
					offset++;
		}

		public Value
		indexValueMask (Bitmask index) {
			int n = offsetFrom (index);
			switch (n) {
				case 0:
					return oneSlopVal:
				case 1:
					return oneSlopVal:
				default:
				return values[n - 2];
			}
		}

		public Measure
		indexMeasureMask (BitMask index) {
			int n = offsetFrom (index);
			switch (n) {
				case 0:
					return oneSlopVal:
				case 1:
					return oneSlopVal:
				default:
				return measure[n - 2];
			}
		}
	}
}
