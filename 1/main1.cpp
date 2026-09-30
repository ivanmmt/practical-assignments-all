/* 
* Task: create class (named IntegerSet) in which I`m implementing
* binary tree which is a set of integers.
* Functional requirements: 
* 1.Creating empty set
* 2.Array collected some quantity elements n (skip duplicate element and pointer on the start array)
* 3.Add constructor copy and move
* 4.Add assignment and move operator
* 5.Add destructor
* 6.Add methods returns the number of stored numbers and determines whether a specified number is present in the set.
* 7.Add overloaded operators
*/

#include <iostream>
#include <functional>
#include <string>
#include <sstream>
using namespace std;

class IntegerSet {
private:
	struct Node {
		int value;
		Node* left;
		Node* right;

		Node(int val) : value(val), left(nullptr), right(nullptr) {}
	};

	Node* root = nullptr;
	size_t count = 0;

	// Helper functions for working with trees
	void clear(Node* node)
	{
		if (!node) return;
		clear(node->left);
		clear(node->right);
		delete node;
	}

	Node* copyTree(const Node* node)
	{
		if (!node) return nullptr;
		Node* newNode = new Node(node->value);
		newNode->left = copyTree(node->left);
		newNode->right = copyTree(node->right);
		return newNode; // if this duplicate
	}

	bool insertNode(Node*& node, int val)
	{
		if (!node)
		{
			node = new Node(val);
			return true;
		}
		if (val < node->value) return insertNode(node->left, val);
		if (val > node->value) return insertNode(node->right, val);
		return false;
	}

	Node* findMin(Node* node) const
	{
		while (node && node->left) node = node->left;
		return node;
	}

	bool removeNode(Node*& node, int val)
	{
		if (!node) return false;
		if (val < node->value) return removeNode(node->left, val);
		if (val > node->value) return removeNode(node->right, val);

		// Node found
		if (!node->left)
		{
			Node* temp = node->right;
			delete node;
			node = temp;
		}
		else if (!node->right)
		{
			Node* temp = node->left;
			delete node;
			node = temp;
		}
		else
		{
			Node* temp = findMin(node->right);
			node->value = temp->value;
			removeNode(node->right, temp->value);
		}
		return true;
	}

	bool containsNode(const Node* node, int val) const
	{
		if (!node) return false;
		if (val == node->value) return true;
		if (val < node->value) return containsNode(node->left, val);
		return containsNode(node->right, val);
	}

	void collectElements(const Node* node, IntegerSet& target) const
	{
		if (!node) return;
		target += node->value;
		collectElements(node->left, target);
		collectElements(node->right, target);
	}


public:
	// 1.CONSTRUCTORS AND DESTRUCTORS

	// default constructor
	IntegerSet() = default;

	// constructor from array
	IntegerSet(size_t n, const int* arr)
	{
		for (size_t i = 0; i < n; i++)
		{
			*this += arr[i];
		}
	}

	// destructor
	~IntegerSet()
	{
		clear(root);
	}

	// copy constructor
	IntegerSet(const IntegerSet& other) : count(other.count)
	{
		root = copyTree(other.root);
	}

	// move constructor
	IntegerSet(IntegerSet&& other) noexcept : root(other.root), count(other.count)
	{
		other.root = nullptr;
		other.count = 0;
	}

	// assignment operator (copying)
	IntegerSet& operator=(const IntegerSet& other) {
		if (this != &other) {
			clear(root);
			root = copyTree(other.root);
			count = other.count;
		}
		return *this;
	}

	// assignment operator (moving)
	IntegerSet& operator=(IntegerSet&& other) noexcept {
		if (this != &other) {
			clear(root);
			root = other.root;
			count = other.count;
			other.root = nullptr;
			other.count = 0;
		}
		return *this;
	}

	// 2.REQUESTS
	size_t size() const
	{
		return count;
	}

	bool contains(int val) const
	{
		return containsNode(root, val);
	}

	// 3.MODIFICATION OPERATORS
	IntegerSet& operator+=(int val) {
		if (insertNode(root, val)) {
			count++;
		}
		return *this;
	}

	IntegerSet& operator-=(int val) {
		if (removeNode(root, val)) {
			count--;
		}
		return *this;
	}

	// 4.MULTIPLE OPERATIONS

	// union (+= / |)
	IntegerSet& operator|=(const IntegerSet& other) {
		other.collectElements(other.root, *this);
		return *this;
	}

	IntegerSet operator|(const IntegerSet& other) const {
		IntegerSet result = *this;
		result |= other;
		return result;
	}

	// intersection (&= / &)
	IntegerSet& operator&=(const IntegerSet& other) {
		IntegerSet result;
		// create new set with all elements for easy
		auto checkAndAdd = [&](auto self, const Node* node) -> void {
			if (!node) return;
			if (other.contains(node->value)) {
				result += node->value;
			}
			self(self, node->left);
			self(self, node->right);
			};
		checkAndAdd(checkAndAdd, root);
		*this = move(result);
		return *this;
	}

	IntegerSet operator&(const IntegerSet& other) const {
		IntegerSet result = *this;
		result &= other;
		return result;
	}

	// difference (-)
	IntegerSet operator-(const IntegerSet& other) const {
		IntegerSet result;
		auto checkAndAdd = [&](auto self, const Node* node) -> void {
			if (!node) return;
			if (!other.contains(node->value)) {
				result += node->value;
			}
			self(self, node->left);
			self(self, node->right);
			};
		checkAndAdd(checkAndAdd, root);
		return result;
	}

	// comparison (==, !=)
	bool operator==(const IntegerSet& other) const {
		if (count != other.count) return false;

		bool isEqual = true;
		auto verify = [&](auto self, const Node* node) -> void {
			if (!node || !isEqual) return;
			if (!other.contains(node->value)) {
				isEqual = false;
				return;
			}
			self(self, node->left);
			self(self, node->right);
			};
		verify(verify, root);
		return isEqual;
	}

	bool operator!=(const IntegerSet& other) const {
		return !(*this == other);
	}
};

int main()
{
	IntegerSet set;
	string line;

	cout << "=== Interactive IntegerSet ===" << endl;
	cout << "Commands:" << endl;
	cout << "  + N   - add number N" << endl;
	cout << "  - N   - remove number N" << endl;
	cout << "  ? N   - check if number N exists" << endl;
	cout << "  print - show set size" << endl;
	cout << "  exit  - exit program" << endl;
	cout << "------------------------------------------" << endl;

	// Infinite input loop
	while (true) {
		cout << "> ";
		if (!getline(cin, line)) {
			break;
		}

		if (line.empty()) continue;

		stringstream ss(line);
		string command;
		ss >> command;

		if (command == "exit") {
			cout << "Exiting program." << endl;
			break;
		}
		else if (command == "+") {
			int val;
			if (ss >> val) {
				set += val;
				cout << "Number " << val << " added. Current size: " << set.size() << endl;
			}
			else {
				cout << "Error: please provide an integer!" << endl;
			}
		}
		else if (command == "-") {
			int val;
			if (ss >> val) {
				size_t oldSize = set.size();
				set -= val;
				if (set.size() < oldSize) {
					cout << "Number " << val << " removed." << endl;
				}
				else {
					cout << "Number " << val << " was not in the set." << endl;
				}
			}
			else {
				cout << "Error: please provide an integer!" << endl;
			}
		}
		else if (command == "?") {
			int val;
			if (ss >> val) {
				if (set.contains(val)) {
					cout << "Number " << val << " is in the set." << endl;
				}
				else {
					cout << "Number " << val << " is missing." << endl;
				}
			}
			else {
				cout << "Error: please provide an integer!" << endl;
			}
		}
		else if (command == "print") {
			cout << "Current set size: " << set.size() << endl;
		}
		else {
			cout << "Unknown command! Use +, -, ?, print or exit." << endl;
		}
	}

	return 0;
}