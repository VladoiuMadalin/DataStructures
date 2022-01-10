#pragma once
#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <iterator>
#include <cstddef>

template<typename keyT, typename valueT, typename cmp = std::less<keyT>>
class map
{
public:
	class node
	{
	public:
		int height;
		std::pair<const keyT, valueT> key_value_pair;
		node* parent, * left, * right;

		node(std::pair<keyT, valueT>key_value_pair, node* parent = nullptr, int height = 0) :
			key_value_pair(key_value_pair), parent(parent), height(height),
			left(nullptr), right(nullptr) {}

		int balance_factor();
		void recalculate_height();
	};

	class iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::pair<const keyT, valueT>;
		using pointer = value_type*;
		using reference = value_type;
	private:
		node* m_node;
		map* m_map;
	public:
		iterator();
		iterator(node* some_node, map* some_map);
		reference operator*() const;
		pointer operator->();
		iterator& operator++();
		iterator operator++(int);
		bool operator==(const iterator& other);
		bool operator!=(const iterator& other);

		friend class map;
	};

protected:
	node* root;
	size_t m_size;

public:
	map();
	~map();
	map(const map& other);
	map& operator=(const map& other);

	iterator begin();
	iterator end();

	void insert(std::pair<keyT, valueT>);
	valueT& operator[](keyT);

	void erase(keyT key);
	void erase(iterator it);
	iterator find(keyT key);

	void clear();

	size_t size();
	bool empty();

protected:

	void rebalance_insert(node*);

	void rotate_left(node*);
	void rotate_right(node*);

	void init(const map& other);
	void erase(node* node);

	node* max(node* root);
	node* min(node* root);

	node* succ(node* node);
	node* pred(node* node);

};



template<typename keyT, typename valueT, typename cmp>
map<keyT, valueT, cmp>::map() : root(nullptr), m_size(0) {}



template<typename keyT, typename valueT, typename cmp>
map<keyT, valueT, cmp>::~map()
{
	if (root)
	{
		std::queue<node*> q;
		q.push(root);

		while (!q.empty())
		{
			if (q.front()->right)
			{
				q.push(q.front()->right);
			}
			if (q.front()->left)
			{
				q.push(q.front()->left);
			}
			delete q.front();
			q.pop();
		}
		root = nullptr;
	}
}

template<typename keyT, typename valueT, typename cmp>
map<keyT, valueT, cmp>::map(const map& other)
{
	init(other);
}

template<typename keyT, typename valueT, typename cmp>
map<keyT, valueT, cmp>& map<keyT, valueT, cmp>::operator=(const map& other)
{
	if (root) this->~map();
	init(other);

	return *this;
}

template<typename keyT, typename valueT, typename cmp>
inline  typename map<keyT, valueT, cmp>::iterator map<keyT, valueT, cmp>::begin()
{
	return iterator(min(root), this);
}

template<typename keyT, typename valueT, typename cmp>
inline  typename map<keyT, valueT, cmp>::iterator map<keyT, valueT, cmp>::end()
{
	return iterator(nullptr, this);
}

template<typename keyT, typename valueT, typename cmp>
void map<keyT, valueT, cmp>::insert(std::pair<keyT, valueT> key_value_pair)
{
	if (root == nullptr)
	{
		root = new node(key_value_pair);
		++m_size;
	}
	else
	{
		node* current_node = root;
		node* prev = nullptr;

		while (current_node != nullptr)
		{
			prev = current_node;

			if (cmp()(key_value_pair.first, current_node->key_value_pair.first))
			{
				current_node = current_node->left;
			}
			else if (key_value_pair.first == current_node->key_value_pair.first)
			{
				return;
			}
			else
			{
				current_node = current_node->right;
			}
		}

		if (cmp()(key_value_pair.first, prev->key_value_pair.first))
		{
			prev->left = new node(key_value_pair, prev);
			current_node = prev->left;
		}
		else
		{
			prev->right = new node(key_value_pair, prev);
			current_node = prev->right;
		}


		rebalance_insert(current_node);
		++m_size;
	}
}

template<typename keyT, typename valueT, typename cmp>
valueT& map<keyT, valueT, cmp>::operator[](keyT key)
{
	if (root == nullptr)
	{
		root = new node(std::make_pair(key, valueT()));
		++m_size;
		return root->key_value_pair.second;
	}
	else
	{
		node* current_node = root;
		node* prev = nullptr;

		while (current_node != nullptr)
		{
			prev = current_node;

			if (cmp()(key, current_node->key_value_pair.first))
			{
				current_node = current_node->left;
			}
			else if (key == current_node->key_value_pair.first)
			{
				return current_node->key_value_pair.second;
			}
			else
			{
				current_node = current_node->right;
			}
		}

		if (cmp()(key, prev->key_value_pair.first))
		{
			prev->left = new node(std::make_pair(key, valueT()), prev);
			current_node = prev->left;
		}
		else
		{
			prev->right = new node(std::make_pair(key, valueT()), prev);
			current_node = prev->right;
		}

		rebalance_insert(current_node);
		++m_size;

		return current_node->key_value_pair.second;
	}
}

template<typename keyT, typename valueT, typename cmp>
void map<keyT, valueT, cmp>::erase(node* this_node)
{
	if (this_node == nullptr) return;

	node*& son = (this_node == root) ? root :
		(this_node->parent->right == this_node ? this_node->parent->right : this_node->parent->left);
	node* rebalance_from = this_node->parent;

	if (!this_node->left && !this_node->right)
	{
		son = nullptr;
	}
	else if (!this_node->left && this_node->right)
	{
		son = this_node->right;
		this_node->right->parent = this_node->parent;
	}
	else if (this_node->left && !this_node->right)
	{
		son = this_node->left;
		this_node->left->parent = this_node->parent;
	}

	else
	{
		node* succ = map::succ(this_node);
		rebalance_from = succ;

		if (succ != this_node->right)
		{
			succ->parent->left = succ->right;
			if (succ->right)
				succ->right->parent = succ->parent;

			succ->right = this_node->right;
			this_node->right->parent = succ;
		}

		succ->left = this_node->left;
		this_node->left->parent = succ;

		succ->parent = this_node->parent;
		son = succ;
	}

	delete this_node;
	--m_size;

	this_node = rebalance_from;
	while (this_node != nullptr)
	{
		this_node->recalculate_height();
		int balance_factor = this_node->balance_factor();

		if (balance_factor == -1 || balance_factor == 1) break;
		else if (balance_factor == -2)
		{
			if (this_node->left->balance_factor() == 1)
			{
				rotate_left(this_node->left);
				rotate_right(this_node);
			}
			else if (this_node->left->balance_factor() == 0)
			{
				rotate_right(this_node);
				break;
			}
			else
			{
				rotate_right(this_node);
			}
		}
		else if (balance_factor == 2)
		{
			if (this_node->right->balance_factor() == -1)
			{
				rotate_right(this_node->right);
				rotate_left(this_node);
			}
			else if (this_node->right->balance_factor() == 0)
			{
				rotate_left(this_node);
				break;
			}
			else
			{
				rotate_left(this_node);
			}

		}
		this_node = this_node->parent;
	}
}


template<typename keyT, typename valueT, typename cmp>
void map<keyT, valueT, cmp>::erase(keyT key)
{
	erase(find(key).m_node);
}

template<typename keyT, typename valueT, typename cmp>
inline void map<keyT, valueT, cmp>::erase(iterator it)
{
	erase(it.m_node);
}

template<typename keyT, typename valueT, typename cmp>
void map<keyT, valueT, cmp>::clear()
{
	this->~map();
	m_size = 0;
}

template<typename keyT, typename valueT, typename cmp>
void map<keyT, valueT, cmp>::init(const map& other)
{
	m_size = other.m_size;
	if (other.root)
	{
		root = new node({ other.root->key,other.root->value });

		std::stack < std::pair<node*, node*>> stack;
		stack.push({ root,other.root });

		while (!stack.empty())
		{
			std::pair<node*, node*> pair = stack.top();
			stack.pop();

			if (pair.second->right)
			{
				pair.first->right = new node({ pair.second->right->key,pair.second->right->value }, pair.first);
				stack.push({ pair.first->right ,pair.second->right });
			}
			if (pair.second->left)
			{
				pair.first->left = new node({ pair.second->left->key ,pair.second->left->value }, pair.first);
				stack.push({ pair.first->left,pair.second->left });
			}

		}
	}
	else root = nullptr;
}

template<typename keyT, typename valueT, typename cmp>
typename map<keyT, valueT, cmp>::iterator map<keyT, valueT, cmp>::find(keyT key)
{
	node* current_node = root;

	while (current_node && current_node->key_value_pair.first != key)
	{
		if (cmp()(key, current_node->key_value_pair.first))
		{
			current_node = current_node->left;
		}
		else
		{
			current_node = current_node->right;
		}
	}

	return iterator(current_node, this);
}

template<typename keyT, typename valueT, typename cmp>
typename map<keyT, valueT, cmp>::node* map<keyT, valueT, cmp>::max(node* root)
{
	if (!root)
	{
		return nullptr;
	}

	while (root->right)
	{
		root = root->right;
	}

	return root;
}


template<typename keyT, typename valueT, typename cmp>
typename map<keyT, valueT, cmp>::node* map<keyT, valueT, cmp>::min(node* root)
{
	if (!root)
	{
		return nullptr;
	}

	while (root->left)
	{
		root = root->left;
	}

	return root;
}


template<typename keyT, typename valueT, typename cmp>
inline void map<keyT, valueT, cmp>::rebalance_insert(node* current_node)
{
	while (current_node != root)
	{
		current_node = current_node->parent;

		current_node->recalculate_height();

		int balance_factor = current_node->balance_factor();

		if (balance_factor == 0) break;
		else if (balance_factor == -2)
		{
			if (current_node->left->balance_factor() == -1)
			{
				rotate_right(current_node);
			}
			else
			{
				rotate_left(current_node->left);
				rotate_right(current_node);
			}
			break;
		}
		else if (balance_factor == 2)
		{
			if (current_node->right->balance_factor() == 1)
			{
				rotate_left(current_node);
			}
			else
			{
				rotate_right(current_node->right);
				rotate_left(current_node);
			}
			break;
		}
	}
}


template<typename keyT, typename valueT, typename cmp>
void map<keyT, valueT, cmp>::rotate_left(node* this_node)
{

	if (this_node->right)
	{
		node* temp = this_node->right->left;

		if (this_node != root)
		{
			if (this_node->parent->right == this_node)
			{
				this_node->parent->right = this_node->right;
			}
			else
			{
				this_node->parent->left = this_node->right;
			}
		}
		else
		{
			root = this_node->right;
		}

		this_node->right->parent = this_node->parent;

		this_node->right->left = this_node;
		this_node->parent = this_node->right;

		this_node->right = temp;
		if (temp) temp->parent = this_node;

		this_node->recalculate_height();
		this_node->parent->recalculate_height();
	}
}


template<typename keyT, typename valueT, typename cmp>
void map<keyT, valueT, cmp>::rotate_right(node* this_node)
{

	if (this_node->left)
	{
		node* temp = this_node->left->right;

		if (this_node != root)
		{
			if (this_node->parent->right == this_node)
			{
				this_node->parent->right = this_node->left;
			}
			else
			{
				this_node->parent->left = this_node->left;
			}
		}
		else
		{
			root = this_node->left;
		}

		this_node->left->parent = this_node->parent;

		this_node->left->right = this_node;
		this_node->parent = this_node->left;

		this_node->left = temp;
		if (temp) temp->parent = this_node;

		this_node->recalculate_height();
		this_node->parent->recalculate_height();
	}
}


template<typename keyT, typename valueT, typename cmp>
inline size_t map<keyT, valueT, cmp>::size()
{
	return m_size;
}


template<typename keyT, typename valueT, typename cmp>
inline bool map<keyT, valueT, cmp>::empty()
{
	return m_size == 0;
}


template<typename keyT, typename valueT, typename cmp>
typename map<keyT, valueT, cmp>::node* map<keyT, valueT, cmp>::succ(node* node)
{
	if (!node)
	{
		return nullptr;
	}

	if (node->right)
	{
		return min(node->right);
	}

	while (node->parent && node->parent->right == node)
	{
		node = node->parent;
	}
	return node->parent;

}


template<typename keyT, typename valueT, typename cmp>
typename map<keyT, valueT, cmp>::node* map<keyT, valueT, cmp>::pred(node* node)
{
	if (!node)
	{
		return nullptr;
	}

	if (node->left)
	{
		return max(node->left);
	}

	while (node->parent && node->parent->left == node)
	{
		node = node->parent;
	}
	return node->parent;
}


template<typename keyT, typename valueT, typename cmp>
int map<keyT, valueT, cmp>::node::balance_factor()
{
	int right_h = right == nullptr ? -1 : right->height;
	int left_h = left == nullptr ? -1 : left->height;
	return right_h - left_h;
}


template<typename keyT, typename valueT, typename cmp>
void map<keyT, valueT, cmp>::node::recalculate_height()
{
	if (!this->left && !this->right) this->height = 0;
	else if (!this->left)  this->height = this->right->height + 1;
	else if (!this->right)  this->height = this->left->height + 1;
	else  this->height = std::max(this->left->height, this->right->height) + 1;
}


template<typename keyT, typename valueT, typename cmp>
inline map<keyT, valueT, cmp>::iterator::iterator() :m_node(nullptr), m_map(nullptr) {}


template<typename keyT, typename valueT, typename cmp>
inline map<keyT, valueT, cmp>::iterator::iterator(node* some_node, map* some_map) : m_node(some_node), m_map(some_map) {}


template<typename keyT, typename valueT, typename cmp>
typename map<keyT, valueT, cmp>::iterator::reference map<keyT, valueT, cmp>::iterator::operator*() const
{
	return m_node->key_value_pair;
}


template<typename keyT, typename valueT, typename cmp>
typename map<keyT, valueT, cmp>::iterator::pointer map<keyT, valueT, cmp>::iterator::operator->()
{
	return &(m_node->key_value_pair);
}


template<typename keyT, typename valueT, typename cmp>
typename map<keyT, valueT, cmp>::iterator& map<keyT, valueT, cmp>::iterator::operator++()
{
	m_node = m_map->succ(m_node);
	return *this;
}


template<typename keyT, typename valueT, typename cmp>
typename map<keyT, valueT, cmp>::iterator map<keyT, valueT, cmp>::iterator::operator++(int)
{
	iterator tmp = *this;
	++(*this);
	return tmp;
}



template<typename keyT, typename valueT, typename cmp>
inline bool  map<keyT, valueT, cmp>::iterator::operator==(const iterator& other)
{
	return this->m_node == other->m_node;
}


template<typename keyT, typename valueT, typename cmp>
inline bool  map<keyT, valueT, cmp>::iterator::operator!=(const iterator& other)
{
	return this->m_node != other.m_node;
}