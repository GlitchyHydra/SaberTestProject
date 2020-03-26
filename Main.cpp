#include "Tests.h"

#include <iostream>

using namespace std;

void getBits(int number) {
	const size_t size = sizeof(int) * 8;
	size_t mask = 1;
	bool bits[size];
	//get bit by mask
	for (int i = 0; i < size; i++) {
		bits[i] = number & mask;
		//shift mask
		mask = mask << 1;
	}
	//print
	for (int i = size - 1; i >= 0; i--) {
		cout << bits[i];
	}
	cout << endl;
	return;
}

int main() {
	prepareSerializationOneElement();
	prepareSerializationEmpty();
	prepareSerialization();
	//testAverageNormals();
	return 0;
}