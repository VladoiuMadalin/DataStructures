#pragma once
#include "priority_queue.h"


template<typename T, typename compare>
void priority_queue<T, compare>::sift_down(int index)
{
	while (true)
	{
		int index_max = index;

		if (left(index) < heap.size() && compare()(heap[index_max], heap[left(index)]))
		{
			index_max = left(index);
		}
		if (right(index) < heap.size() && compare()(heap[index_max], heap[right(index)]))
		{
			index_max = right(index);
		}
		if (index == index_max)
		{
			break;
		}

		std::swap(heap[index], heap[index_max]);
		index = index_max;
	}
}

template<typename T, typename compare>
void priority_queue<T, compare>::sift_up(int index)
{
	while (index != 0 && compare()(heap[parent(index)], heap[index]))
	{
		std::swap(heap[index], heap[parent(index)]);
		index = parent(index);
	}
}


template<typename T, typename compare>
inline bool priority_queue<T, compare>::empty() const
{
	return heap.empty();
}

template<typename T, typename compare>
int priority_queue<T, compare>::size() const
{
	return heap.size();
}

template<typename T, typename compare>
void priority_queue<T, compare>::print_heap() const
{
	for (int element : heap)
	{
		std::cout << element << " ";
	}
}

template<typename T, typename compare>
inline T priority_queue<T, compare>::top() const
{
	if (!heap.empty())
	{
		return heap[0];
	}
}

template<typename T, typename compare>
void priority_queue<T, compare>::push(T element)
{
	heap.push_back(element);
	sift_up(heap.size() - 1);
}

template<typename T, typename compare>
void priority_queue<T, compare>::pop()
{
	if (heap.size() > 0)
	{
		heap[0] = heap[heap.size() - 1];
		heap.pop_back();
		sift_down(0);
	}
}

