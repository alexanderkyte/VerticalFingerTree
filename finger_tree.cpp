#include <bitset>
#include "finger_tree.h"

template <class A, unsigned int N>
FingerTree<A, N>
FingerTree<A, N>::pushLeft (const void *elem) const
{
	const int left_state = get_state(0, this->vec);
	const int right_state = get_state(3, this->vec);

	if(left_state == 0 && right_state < 3) {
		switch(right_state) {
			case 0: {
				diff->right.slop = elem;
				break;
			}
			case 1: {
				// Since we're pushing to the empty far side,
				// we
				diff->right.affix = elem;
				diff->right.slop = this->data;
				break;
			}
			case 2: {
				diff->right.affix = this->data;
				break;
			}
		}
	} else {
		switch(left_state) {
			case 0: {
				diff->left.slop = elem;
				break;
			}
			case 1: {
				// Since we're pushing to the empty far side,
				// we
				diff->left.affix = elem;
				// old slop
				diff->left.slop = this->left_data;
				break;
			}
			case 2: {
				// layout in array is
				// | affix | spine |
				const void *affix = this->left_data[0];
				const void *spine = this->left_data[1];
				const Node *node = make_node(elem, affix, spine);
				break;
			}
			case 4: {
				printf("Handle the left push overflow case!");
				// append overflow on left
			}
		}
	}

	int upper = __builtin_popcount(this->vec << 6);

	return make_tree(tree->vec, diff, upper);
}

void 
FingerTree::popLeft ()
{
	const int left_state = get_state(0, tree->vec);
	const int right_state = get_state(3, tree->vec);

	Diff diff = {0};

	if(left_state == 0 && right_state < 3) {
		switch(right_state) {
			case 0: {
				assert(0 && "Trying to pop empty tree, handle this.");
			}
			case 1: {
				diff->right->bitmap = ~0x1;
				break;
			}
			case 2: {
				diff->right.affix = tree->data;
				break;
			}
		}
	} else {
		switch(left_state) {
			case 0: {
				diff->left.slop = elem;
				break;
			}
			case 1: {
				// Since we're pushing to the empty far side,
				// we
				diff->left.affix = elem;
				// old slop
				diff->left.slop = tree->left_data;
				break;
			}
			case 2: {
				// layout in array is
				// | affix | spine |
				const void *affix = &tree->left_data[0];
				const void *spine = &tree->left_data[1];
				const Node *node = make_node(elem, affix, spine);
				break;
			}
			case 4: {
				assert(0 && "Handle the left push overflow case!");
				// append overflow on left
			}
		}
	}

	int upper = __builtin_popcount(tree->vec << 6);

	return make_tree(tree->vec, diff, upper);
	
	return do_action (tree, 0, LEFT_SIDE_PULL);
}

const void *
FingerTree::peekLeft() 
{
	const int left_state = get_state(0, tree->vec);
	const int right_state = get_state(3, tree->vec);

	switch(left_state) {
		case 0:
			break;
		case 4: 
		case 1:
		case 2:
			// Return slop value
			return this->data[0];
		case 3: {
			// Return leftmost data in node
			Node<A> oneNode = (Node<A>)(this->data[0]);
			return oneNode->left;
		}
	}

	// Left tree empty
	// Else need to pull from right,
	// mostly empty tree
	switch(right_state) {
		case 0:
			assert(0 && "shouldn't be reached");
			break;
		case 1:
			// Return slop value
			return this->data[0];
		case 2:
			// Return affix value
			return this->data[1];
		case 3: 
		case 4: {
			// Return leftmost data in node
			Node<A> oneNode = (Node<A>)(this->data[0]);
			return oneNode->left;
		}
	}
}

static inline
const void **
append_level(void **data, Diff newState) {
	int level_index = 0;

	const int num_inner = newState->left_count + newState->right_count;
	if(newState->left_slop) {
		data[level_index++] = newState->left_slop;
	}
	if(newState->left_affix) {
		data[level_index++] = newState->left_affix;
	}
	if(newState->right_slop) {
		data[level_index++] = newState->right_slop;
	}
	if(newState->right_affix) {
		data[level_index++] = newState->right_affix;
	}

	return &data[level_index];
}

static inline
const FingerTree *
make_tree(FingerTree *tree, int below, Diff newState, int above) 
{
	const size_t tree_size = newState->left_count +
		newState->right_count + above;

	FingerTree *out = calloc(1, sizeof(FingerTree));
	out->bitmap = new std::bitset<32>(tree->bitmap);
	out->data = calloc(tree_size, sizeof(void *));
	void **rest = append_level(out->data);

	memcpy(&rest, &tree->data[tree->size - above], above * sizeof(void *));

	assert((rest + above) / sizeof(void *) == tree_size);

	return out;
}


