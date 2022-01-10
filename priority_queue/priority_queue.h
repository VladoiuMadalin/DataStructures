#include <iostream>
#include <vector>
#include "basic_comparators.cpp"
#include "heap_functions.cpp"

template <typename T, typename compare = greater<T>>
class priority_queue
{
private:
	std::vector<T> heap;
	void sift_down(int index);
	void sift_up(int index);

public:
	void print_heap() const;
	T top() const;
	void push(T element);
	void pop();
	bool empty() const;
	int size() const;
};