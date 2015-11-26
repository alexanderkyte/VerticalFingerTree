#ifndef __FINGER_TREE_H
#define __FINGER_TREE_H

#include "succ_array.hpp"

typedef void ** pointer;

class FingerNode {
	public:
		const pointer left;
		const pointer middle;
		const pointer right;

		FingerNode(const pointer left, const pointer middle, const pointer right):
			left(left), middle(middle), right(right) {};
};

template <typename Bitmask>
class FingerTree {
	typedef SuccinctArray<Bitmask, pointer> Spine;

	private:
		const Spine left;
		const Spine right;

		FingerTree(const Spine *left, const Spine *right):
			left(*left), right(*right) {}

		const inline pointer
		peek(const Spine &near, const Spine &far) const;

	public:
		FingerTree(void): left(), right() {}

		const FingerTree<Bitmask> *
		pushRight(const pointer elem) const;

		const pointer
		peekRight() const;

		const FingerTree<Bitmask> 
		popRight(void) const;

		const FingerTree<Bitmask> *
		pushLeft(const pointer elem) const;

		const FingerTree<Bitmask> 
		popLeft(void) const;

		const pointer 
		peekLeft() const;
};

//
// Push
//
template <typename Bitmask>
const FingerTree<Bitmask> *
FingerTree<Bitmask>::push(const bool left, const pointer elem) const
{
	const SuccinctArray<Bitmask, pointer> near_tmp;
	const SuccinctArray<Bitmask, pointer> far_tmp;

	if(left) {
		near_tmp = this->left;
		far_tmp = this->right;
	} else {
		far_tmp = this->left;
		near_tmp = this->right;
	}

	const SuccinctArray<Bitmask, pointer> near = near_tmp;
	const SuccinctArray<Bitmask, pointer> far = far_tmp;

	const int near_state = near.getState(0);
	const int far_state = far.getState(0);

	if(near_state == 0 && far_state < 2) {
		SuccinctArray<Bitmask, pointer> *newFar;
		const Level<pointer> curr = far.getHeadLevel();
		const Level<pointer> next = curr.add(left, near_state, elem);
		const auto newFar = near.setHeadLevel(far_state + 1, next);
		if(left) {
			return new FingerTree(near, newFar);
		} else {
			return new FingerTree(newFar, near);
		}
	} else {
		SuccinctArray<Bitmask, pointer> *newNear;
		if(near_state == 4) {
				newFar = near.setHeadLevel(near_state + 1, next);
		} else {
				const Level<pointer> curr = near.getHeadLevel();
				const Level<pointer> next = curr.add(left, near_state, elem);
				newFar = near.setHeadLevel(near_state + 1, next);
		}
		if(left) {
			return new FingerTree(far, newNear);
		} else {
			return new FingerTree(newNear, far);
		}
	}
}

template <typename Bitmask>
const FingerTree<Bitmask> *
FingerTree<Bitmask>::pushRight(const pointer elem) const
{
	const int left_state = this->left.getState(0);
	const int right_state = this->right.getState(0);

	if(right_state == 0 && right_state < 2) {
		switch(left_state) {
			case 0: {
				// Note, the funny address of syntax is to use
				// the member function
				const SuccinctArray<Bitmask, pointer> *newLeft =
					this->left.setHeadLevel(1, nullptr, elem);

				return new FingerTree(newLeft, &right);
			}
			case 1: {
				Level<pointer> curr = right.getHeadLevel();
				auto newLeft = right.setHeadLevel(2, elem, curr.slop);
				return new FingerTree(newLeft, &right);
			}
			default:
				assert(0 && "Should not be reached");
		}
	} else {
		switch(left_state) {
			case 0: {
				auto newRight = this->right.setHeadLevel(1, nullptr, elem);
				return new FingerTree(&left, newRight);
			}
			case 1: {
				Level<pointer> curr = this->right.getHeadLevel();
				auto newRight = this->right.setHeadLevel(2, curr.slop, elem);
				return new FingerTree(&left, newRight);
			}
			case 2: {
				Level<pointer> curr = this->right.getHeadLevel();
				const FingerNode *node = new FingerNode(curr.affix, curr.slop, elem);
				auto newRight = this->right.setHeadLevel(2, (pointer)node, nullptr);
				return new FingerTree(&left, newRight);
			}
			case 3: {
				Level<pointer> curr = this->right.getHeadLevel();
				auto newRight = this->right.setHeadLevel(4, curr.affix, elem);
				return new FingerTree(&left, newRight);
			}
			case 4: {
				assert(0 && "Handle the left push overflow case!");
				return nullptr;
			}
			default:
				assert(0 && "Should not be reached");
		}
	}
}

//
// Pop
//
template <typename Bitmask>
const FingerTree<Bitmask> 
FingerTree<Bitmask>::popLeft(void) const
{
	const int left_state = this->left.getState(0);
	const int right_state = this->right.getState(0);

	if(left_state == 0) {
		switch(right_state) {
			case 0: {
				assert(0 && "Trying to pop empty tree, handle this->");
				return nullptr;
			}
			case 1: {
				return new FingerTree(nullptr, nullptr);
			}
			case 2: {
				Level<pointer> curr = this->right.getHeadLevel();
				auto newRight = this->right.setHeadLevel(1, nullptr, curr.slop);
				return new FingerTree(this->left, newRight);
			}
		}
	} else {
		switch(left_state) {
			case 1: {
				assert(0 && "Handle the left pop underflow case!");
				return nullptr;
			}
			case 2: {
				Level<pointer> curr = this->left.getHeadLevel();
				auto newLeft = this->left.setHeadLevel(1, nullptr, curr.affix);
				return new FingerTree(nullptr, this->right);
			}
			case 3: {
				Level<pointer> curr = this->left.getHeadLevel();
				FingerNode *node = (FingerNode *)curr.affix;
				auto newLeft = this->left.setHeadLevel(2, node->middle, node->right);
				return new FingerTree(newLeft, this->right);
			}
			case 4: {
				Level<pointer> curr = this->left.getHeadLevel();
				auto newLeft = this->left.setHeadLevel(3, curr.affix, nullptr);
				return new FingerTree(newLeft, this->right);
			}
		}
	}
}

template <typename Bitmask>
const FingerTree<Bitmask> 
FingerTree<Bitmask>::popRight(void) const
{
	const int left_state = this->left.getState(0);
	const int right_state = this->right.getState(0);

	if(right_state == 0) {
		switch(left_state) {
			case 0: {
				assert(0 && "Trying to pop empty tree, handle this->");
				return nullptr;
			}
			case 1: {
				return new FingerTree(nullptr, nullptr);
			}
			case 2: {
				Level<pointer> curr = this->left.getHeadLevel();
				auto newLeft = this->left.setHeadLevel(1, nullptr, curr.slop);
				return new FingerTree(newLeft, this->right);
			}
		}
	} else {
		switch(left_state) {
			case 1: {
				assert(0 && "Handle the left pop underflow case!");
				return nullptr;
			}
			case 2: {
				Level<pointer> curr = this->right.getHeadLevel();
				auto newRight = this->right.setHeadLevel(1, nullptr, curr.affix);
				return new FingerTree(this->left, newRight);
			}
			case 3: {
				Level<pointer> curr = this->right.getHeadLevel();
				FingerNode *node = (FingerNode *)curr.affix;
				auto newRight = this->right.setHeadLevel(2, node->left, node->middle);
				return new FingerTree(this->left, newRight);
			}
			case 4: {
				Level<pointer> curr = this->right.getHeadLevel();
				auto newRight = this->right.setHeadLevel(3, curr.affix, nullptr);
				return new FingerTree(this->left, newRight);
			}
		}
	}
}

//
// Peek
//


template <typename Bitmask>
const inline pointer
FingerTree<Bitmask>::peekLeft(void) const 
{
	const int left_state = this->left.getState(0);
	const int right_state = this->right.getState(0);

	if(left_state == 0) {
		Level<pointer> curr = right.getHeadLevel();
		switch(right_state) {
			case 0:
				return {0};
			case 1:
				return curr.slop;
			case 2:
				return curr.slop;
			default:
				assert(0 && "Should not be reached");
				return {0};
		}
	} else {
		Level<pointer> curr = left.getHeadLevel();
		switch(left_state) {
			case 0:
				return {0};
			case 4: 
			case 1:
			case 2:
				return curr.slop;
			case 3:
				return ((FingerNode *)curr.affix)->left;
			default:
				assert(0 && "Should not be reached");
				return {0};
		}
	}
}

template <typename Bitmask>
const inline pointer
FingerTree<Bitmask>::peekRight(void) const 
{
	const int left_state = this->left.getState(0);
	const int right_state = this->right.getState(0);

	if(right_state == 0) {
		Level<pointer> curr = left.getHeadLevel();
		switch(left_state) {
			case 0:
				return {0};
			case 1:
				return curr.slop;
			case 2:
				return curr.affix;
			default:
				assert(0 && "Should not be reached");
				return {0};
		}
	} else {
		Level<pointer> curr = right.getHeadLevel();
		switch(right_state) {
			case 0:
				return {0};
			case 4: 
			case 1:
			case 2:
				return curr.slop;
			case 3:
				return ((FingerNode *)curr.affix)->right;
			default:
				assert(0 && "Should not be reached");
				return {0};
		}
	}
}


#endif
