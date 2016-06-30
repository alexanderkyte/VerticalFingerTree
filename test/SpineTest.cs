using Spine;
using System;

public namespace SpineTest {

	public class SpineTest {
		public static void assert(bool cond) {
			if (!cond)
				throw new Exception ();
		}

		public static void Main (string[] args) 
		{
			Test1();
		}

		public void Test1() {
			var spine = new Spine <Bitmask, String>();
			Console.WriteLine (spine.AssemblyQualifiedName);
		}
	}
}
