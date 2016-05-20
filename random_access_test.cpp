#include "random_access.hpp"
#include "iostream"

int main(void) {
	PersistentSequence<int> *fibs = new PersistentSequence<int>();

	for(int i=0; i < 30; i++) {
		int first = fibs->get(i-1);
		int second = fibs->get(i-2);
		int next = first + second;
		fibs->append(next);
	}

	std::cout << "Backwards fibs";

	for(int i=0; i < 30; i++) {
		std::cout << i << ": " << fibs->get(29-i) << "\n";
	}
}
