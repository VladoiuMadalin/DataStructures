#pragma once
#include <iostream>
#include <string>
#include <stack>
#include <queue>
class AVL
{
public:
	enum class print_mode
	{
		preorder = 1,
		inorder,
		postorder,
		tree
	};

	class node
	{
	public:
		int key, height;
		node* parent, * left, * right;
		node(int key, node* parent = nullptr, int height = 0) :key(key), parent(parent), height(height),
			left(nullptr), right(nullptr) {}
		int balance_factor();
		void recalculate_height();
	};

protected:
	node* root;

public:
	AVL();
	AVL(int*, size_t size);
	~AVL();
	AVL(const AVL& other);
	AVL& operator=(const AVL& other);

	void insert(int key);
	void remove(int key);

	void clear();

	int max();
	int min();

	void print(print_mode mode);
	int succ(int key);
	int pred(int key);
	bool find(int key);

protected:

	void print_preorder();
	void print_inorder();
	void print_postorder();
	void print_tree();

	void rotate_left(node*);
	void rotate_right(node*);

	void init(const AVL& other);
	node* search(int key);
	void remove(node* node);

	node* max(node* root);
	node* min(node* root);

	node* succ(node* node);
	node* pred(node* node);

};