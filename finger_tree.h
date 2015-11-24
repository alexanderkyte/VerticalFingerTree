#ifndef __FINGER_TREE_H
#define __FINGER_TREE_H

template <class A, unsigned int N>
class FingerTree {
	private:
		PersistentBunchedArray left;
		PersistentBunchedArray right;

	public:
		//FingerTree pushRight(A data) const;
		//A peekRight() const;
		//FingerTree popRight() const;

		FingerTree<A, N> pushLeft(const void *data) const;
		A peekLeft() const;
		FingerTree<A, N> popLeft() const;
};

#endif
