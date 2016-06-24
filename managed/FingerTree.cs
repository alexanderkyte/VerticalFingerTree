
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
}
