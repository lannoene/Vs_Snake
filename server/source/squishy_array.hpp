#pragma once

#include <iostream>
#include <cstdlib>

template <typename type> class SquishyArray {
public:
	SquishyArray(int inputArrayLength) {
		array_ptr = (type*)malloc(inputArrayLength * sizeof(type));
		array_length = inputArrayLength;
		if (array_ptr == nullptr) {
			puts("Your array pointer is null. This is not gud lul XD lamo rofl");
			exit(255);
		}
	}
	~SquishyArray(void) {
		free(array_ptr);
	}
	size_t length(void) {
		return array_length;
	}
	void array_push(type value) {
		//array_capacity = array_capacity == 0 ? array_capacity << 1 : 1;
		change_array_size(1);
		array_ptr[array_length - 1] = value;
	}
	type* data(void) {
		return array_ptr;
	}
	void array_front(type value) {
		change_array_size(1);
		for (type i = array_length - 1; i > 0; i--) {
			array_ptr[i] = array_ptr[i - 1];
		}
		array_ptr[0] = value;
	}
	void array_splice(size_t index, int elements) {
		if (index > array_length || index + elements > array_length) {
			return;
		}
		for (size_t i = index; i < array_length - elements; i++) {
			array_ptr[i] = array_ptr[i + elements];
		}
		array_ptr = (type*)realloc(array_ptr, (array_length - elements) * sizeof(type));
		array_length -= elements;
	}
	size_t shortLen() {
		return array_length - 1;
	}
private:
	type* array_ptr;
	size_t array_length;
	size_t array_capacity;
	
	void change_array_size(signed long long newSize) {
		array_length += newSize;
		array_ptr = (type*)realloc(array_ptr, array_length * sizeof(type));
	}
};