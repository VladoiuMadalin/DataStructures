#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <string>


template<typename key, typename value, typename hash = std::hash<key>>
class hashtable
{
private:
	std::list<std::pair<key, value>>* arr;
	size_t arr_length;
	size_t number_of_pairs;

	void rehash();
	size_t get_index(key);
	void init(const hashtable& other);
public:
	hashtable();
	hashtable(const hashtable& other);
	hashtable<key, value, hash> operator=(const hashtable& other);
	~hashtable();


	void insert(std::pair<key, value>);
	value get_value(key);
	float load_factor();
	void print();
	bool find(key);
	void remove(key);
	value& operator[](key);
};




template<typename key, typename value, typename hash>
inline hashtable<key, value, hash>::hashtable()
{
	number_of_pairs = 0;
	arr_length = 7;
	arr = new std::list<std::pair<key, value>>[arr_length];
}

template<typename key, typename value, typename hash>
inline hashtable<key, value, hash>::hashtable(const hashtable& other)
{
	init(other);
}

template<typename key, typename value, typename hash>
inline hashtable<key, value, hash> hashtable<key, value, hash>::operator=(const hashtable& other)
{
	if (arr != nullptr) this->~hashtable();
	init();

	return *this;
}



template<typename key, typename value, typename hash>
inline hashtable<key, value, hash>::~hashtable()
{
	delete[] arr;
}


template<typename key, typename value, typename hash>
inline void hashtable<key, value, hash>::insert(std::pair<key, value> pair)
{
	++number_of_pairs;
	if (load_factor() >= 1) rehash();

	if (!find(pair.first))
	{
		size_t index = get_index(pair.first);
		arr[index].emplace_back(pair);
	}
}


template<typename key, typename value, typename hash>
inline float hashtable<key, value, hash>::load_factor()
{
	return number_of_pairs / arr_length;
}

template<typename key, typename value, typename hash>
inline void hashtable<key, value, hash>::print()
{
	for (int index = 0; index < arr_length; ++index)
	{
		std::cout << index << " : ";

		for (std::pair<key, value> pair : arr[index])
		{
			std::cout << "{" << pair.first << ", " << pair.second << "} ";
		}
		std::cout << "\n";
	}
}

template<typename key, typename value, typename hash>
inline bool hashtable<key, value, hash>::find(key current_key)
{
	size_t index = get_index(current_key);

	for (std::pair<key, value> pair : arr[index])
	{
		if (pair.first == current_key)
		{
			return true;
		}
	}
	return false;
}

template<typename key, typename value, typename hash>
inline void hashtable<key, value, hash>::remove(key current_key)
{
	size_t index = get_index(current_key);
	auto itr = arr[index].begin();

	while (itr != arr[index].end())
	{
		if (itr->first == current_key)
		{
			arr[index].erase(itr);
			--arr_length;
			break;
		}
		itr++;
	}
}

template<typename key, typename value, typename hash>
value& hashtable<key, value, hash>::operator[](key current_key)
{
	size_t index = get_index(current_key);

	for (auto itr = (arr[index]).begin(); itr != arr[index].end(); itr++)
	{
		if (itr->first == current_key)
		{
			return itr->second;
		}
	}

	std::pair<key, value> pair;
	pair.first = current_key;

	arr[index].emplace_back(pair);
	return arr[index].rbegin()->second;
}


template<typename key, typename value, typename hash>
inline void hashtable<key, value, hash>::rehash()
{
	size_t old_arr_length = arr_length;
	arr_length = arr_length * 2 - 1;
	std::list<std::pair<key, value>>* temp = new std::list<std::pair<key, value>>[arr_length];

	for (int index = 0; index < old_arr_length; ++index)
	{
		for (auto pair : arr[index])
		{
			int new_index = get_index(pair.first);
			temp[new_index].emplace_back(pair);
		}
	}
	delete[] arr;
	arr = temp;
}

template<typename key, typename value, typename hash>
size_t hashtable<key, value, hash>::get_index(key current_key)
{
	return hash()(current_key) % arr_length;
}

template<typename key, typename value, typename hash>
void hashtable<key, value, hash>::init(const hashtable& other)
{
	arr_length = other.arr_length;
	number_of_pairs = other.number_of_pairs/*)*/;
	arr = new std::list<std::pair<key, value>>[arr_length];
	for (size_t index = 0; index < arr_length; ++index)
	{
		arr[index] = other.arr[index];
	}
}

template<typename key, typename value, typename hash>
inline value hashtable<key, value, hash>::get_value(key current_key)
{
	size_t index = get_index(current_key);

	for (std::pair<key, value> pair : arr[index])
	{
		if (pair.first == current_key)
		{
			return pair.second;
		}
	}
	return value();
}