#include "AVL.h"

AVL::AVL() : root(nullptr) {}

AVL::AVL(int* arr, size_t size)
{
	for (size_t index = 0; index < size; index++)
	{
		insert(arr[index]);
	}
}

AVL::node* AVL::search(int key)
{
	node* current_node = root;

	while (current_node && current_node->key != key)
	{
		if (key < current_node->key)
		{
			current_node = current_node->left;
		}
		else
		{
			current_node = current_node->right;
		}
	}

	return current_node;
}

AVL::~AVL()
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

AVL::AVL(const AVL& other)
{
	init(other);
}

AVL& AVL::operator=(const AVL& other)
{
	if (root) this->~AVL();
	init(other);

	return *this;
}

void AVL::insert(int key)
{
	if (root == nullptr)
	{
		root = new node(key);
	}
	else
	{
		node* current_node = root;
		node* prev = nullptr;

		while (current_node != nullptr)
		{
			prev = current_node;

			if (key < current_node->key)
			{
				current_node = current_node->left;
			}
			else
			{
				current_node = current_node->right;
			}
		}

		if (key < prev->key)
		{
			prev->left = new node(key, prev);
			current_node = prev->left;
		}
		else
		{
			prev->right = new node(key, prev);
			current_node = prev->right;
		}


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
}

void AVL::remove(node* this_node)
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
		node* succ = AVL::succ(this_node);
		rebalance_from = succ;

		if (succ != this_node->right)
		{
			//rebalance_from = succ->parent;
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



void AVL::remove(int key)
{
	remove(search(key));
}

void AVL::clear()
{
	this->~AVL();
}

void AVL::print_tree()
{
	struct state
	{
		node* node = nullptr;
		char side = 0;
		std::string prefix;
	};

	std::string extra_spaces;
	std::stack<state> memory_stack;
	state state;

	memory_stack.push({ root, 'L',"" });

	while (!memory_stack.empty())
	{
		state = memory_stack.top();
		memory_stack.pop();



		std::cout << state.prefix;

		if (state.side == 'L')
		{
			std::cout << "`--(";
		}
		else if (state.side == 'R')
		{
			std::cout << "|--(";
		}

		extra_spaces = "";
		if (state.node)
		{
			std::cout << state.node->key;

			std::cout << ")" << "\n";



			if (state.node->right || state.node->left)
			{
				memory_stack.push({ state.node->left,'L',state.prefix + (state.side == 'L' ? "      " : "|     ") + extra_spaces });
				memory_stack.push({ state.node->right,'R',state.prefix + (state.side == 'L' ? "      " : "|     ") + extra_spaces });
			}
		}
		else
		{
			std::cout << "nll)\n";
		}

	}
}

void AVL::init(const AVL& other)
{
	if (other.root)
	{
		root = new node(other.root->key);

		std::stack < std::pair<node*, node*>> stack;
		stack.push({ root,other.root });

		while (!stack.empty())
		{
			std::pair<node*, node*> pair = stack.top();
			stack.pop();

			if (pair.second->right)
			{
				pair.first->right = new node(pair.second->right->key, pair.first);
				stack.push({ pair.first->right ,pair.second->right });
			}
			if (pair.second->left)
			{
				pair.first->left = new node(pair.second->left->key, pair.first);
				stack.push({ pair.first->left,pair.second->left });
			}

		}
	}
	else root = nullptr;
}


AVL::node* AVL::max(node* root)
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

int AVL::max()
{
	if (root) return max(root)->key;
	return INT_MIN;
}

AVL::node* AVL::min(node* root)
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


void AVL::rotate_left(node* this_node)
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

void AVL::rotate_right(node* this_node)
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

int AVL::min()
{
	if (root) return min(root)->key;
	return INT_MAX;
}

void AVL::print(print_mode mode)
{
	switch (mode)
	{
	case AVL::print_mode::preorder:
		this->print_preorder();
		break;
	case AVL::print_mode::inorder:
		this->print_inorder();
		break;
	case AVL::print_mode::postorder:
		this->print_postorder();
		break;
	case AVL::print_mode::tree:
		this->print_tree();
		break;
	}
}

int AVL::succ(int key)
{
	node* succ = AVL::succ(search(key));
	if (succ == nullptr)
	{
		return INT_MIN;
	}
	return succ->key;
}

int AVL::pred(int key)
{
	node* pred = AVL::pred(search(key));
	if (pred == nullptr)
	{
		return INT_MAX;
	}
	return pred->key;
}

bool AVL::find(int key)
{
	if (search(key) == nullptr)
	{
		return false;
	}
	return true;
}




void AVL::print_preorder()
{
	std::stack<node*> stack;
	stack.push(root);

	while (!stack.empty())
	{
		node* this_node = stack.top();
		std::cout << this_node->key << " ";
		stack.pop();

		if (this_node->right)
		{
			stack.push(this_node->right);
		}
		if (this_node->left)
		{
			stack.push(this_node->left);
		}
	}
	std::cout << "\n";
}

void AVL::print_inorder()
{
	struct state
	{
		node* this_node;
		bool expanded_left;
	};

	std::stack<state> stack;
	stack.push({ root,false });


	while (!stack.empty())
	{
		node* this_node = stack.top().this_node;
		if (!stack.top().expanded_left)
		{
			stack.top().expanded_left = true;
			if (this_node->left)
			{
				stack.push({ this_node->left ,false });
			}
		}
		else
		{
			stack.pop();
			std::cout << this_node->key << " ";

			if (this_node->right)
			{
				stack.push({ this_node->right ,false });
			}
		}

	}
	std::cout << "\n";
}

void AVL::print_postorder()
{
	struct state
	{
		node* this_node;
		char code; //X->not expanded  L->expanded left  R->expanded left and right
	};

	std::stack<state> stack;
	stack.push({ root,'X' });


	while (!stack.empty())
	{
		node* this_node = stack.top().this_node;


		switch (stack.top().code)
		{
		case 'X':
			stack.top().code = 'L';
			if (this_node->left)
			{
				stack.push({ this_node->left ,'X' });
			}
			break;
		case 'L':
			stack.top().code = 'R';
			if (this_node->right)
			{
				stack.push({ this_node->right ,'X' });
			}
			break;
		case 'R':
		{
			std::cout << this_node->key << " ";
			stack.pop();
			break;
		}
		default:
			break;
		}

	}
	std::cout << "\n";
}

AVL::node* AVL::succ(node* node)
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

AVL::node* AVL::pred(node* node)
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

int AVL::node::balance_factor()
{
	int right_h = right == nullptr ? -1 : right->height;
	int left_h = left == nullptr ? -1 : left->height;
	return right_h - left_h;
}

void AVL::node::recalculate_height()
{
	if (!this->left && !this->right) this->height = 0;
	else if (!this->left)  this->height = this->right->height + 1;
	else if (!this->right)  this->height = this->left->height + 1;
	else  this->height = std::max(this->left->height, this->right->height) + 1;
}
