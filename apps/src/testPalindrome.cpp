#include <string>
#include <iostream>
#include <list>
#include <ranges>
#include <vector>
#include <algorithm>

using namespace std;

template<typename T=int>
struct Node {
    T val;
    Node<T>* left;
    Node<T>* right;

    explicit Node(T val, Node<T>* left = nullptr, Node<T>* right = nullptr)
        : val{val}, left{left}, right{right} {}

    ~Node() {
        delete left;
        delete right;
    }
};
//-----------
bool isPalindrome(const string& str)
{
    for(int i=0; i <str.size(); i++)
    {
        if(str[i] != str[str.size() -i -1]) return false;
    }
    return true;
}
int makePalindrome(string&  str)
{
    int left = 0;
    int right = str.size() - 1;
    int counter = 0;
    for(int i=0; i <str.size(); i++)
    {
        auto first = str.begin();
        auto last = str.rbegin();
        advance(first, i);
        advance(last, i);
        if(*first != *last)
        {
            str.insert(first, *last);
            counter++;
            if(isPalindrome(str))
                break;
        }
    }
    cout << str << endl;
    return counter;
}

// is same tree
bool is_same_tree(Node<int>* tree1, Node<int>* tree2) {
    if (!tree1 && !tree2) return true;
    if (!tree1 || !tree2) return false;
    return tree1->val == tree2->val &&
            is_same_tree(tree1->left, tree2->left) &&
            is_same_tree(tree1->right, tree2->right);
}
bool subtree_of_another_tree(Node<int>* root, Node<int>* sub_root) {
    if (!root) return false;
    return is_same_tree(root, sub_root) || subtree_of_another_tree(root->left, sub_root) || subtree_of_another_tree(root->right, sub_root);
}

// is balanced tree 
// Returns the height of the binary tree or -1 if it is not a balanced binary tree
int tree_height(Node<int>* tree) {
    if (tree == nullptr) return 0;
    int left_height = tree_height(tree->left);
    int right_height = tree_height(tree->right);
    if (left_height == -1 || right_height == -1) return -1;
    if (std::abs(left_height - right_height) > 1) return -1;
    return std::max(left_height, right_height) + 1;
}
bool is_balanced(Node<int>* tree) {
    return tree_height(tree) != -1;
}

// max depth
int dfs(Node<int>* root) {
    // Null node adds no depth
    if (root == nullptr) return 0;
    // num nodes in longest path of current subtree = max num nodes of its two subtrees + 1 current node
    return std::max(dfs(root->left), dfs(root->right)) + 1;
}
int tree_max_depth(Node<int>* root) {
    return root ? dfs(root) - 1 : 0;
}

// Visible tree nodes - max value is the current one than parents
int dfs(Node<int>* root, int max_sofar) {
    if (!root) return 0;
    int total = 0;
    if (root->val >= max_sofar) total++;
    total += dfs(root->left, std::max(max_sofar, root->val));
    total += dfs(root->right, std::max(max_sofar, root->val));
    return total;
}
int visible_tree_node(Node<int>* root) {
    // start max_sofar with smallest number possible so any value root has is greater than it
    return dfs(root, std::numeric_limits<int>::min());
}

// invert a tree
void invert(Node<int>* tree)
{
    if(!tree) return;
    Node<int>* temp = tree->right;
    tree->right = tree->left;
    tree->left = temp;
    invert(tree->left);
    invert(tree->right);    
    return;
}
Node<int>* invert_binary_tree(Node<int>* tree) {
    invert(tree);
    return tree;
}

int main(int , char** )
{
    string test = "test";
    cout << makePalindrome(test) << endl;
}