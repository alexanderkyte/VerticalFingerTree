#include <random_access.hpp>

int main(void) {
	PersistentSequence<int> fibs = new PersistentSequence<int>();

	for(int i=0; i < 30; i++) {
		int next = fibs[i-1] + fibs[i-2];
		fibs.pushRight(next);
	}

	cout << "Backwards fibs";

	for(int i=0; i < 30; i++) {
		cout << i << ": " << fibs[29-i] << "\n";
	}
}
