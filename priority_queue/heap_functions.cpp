#include "heap_functions.h"

int left(int index)
{
	return index * 2 + 1;
}

int right(int index)
{
	return index * 2 + 2;
}

int parent(int index)
{
	return (index - 1) / 2;
}