#include <bits/stdc++.h>

using namespace std;

template<typename T=int>
struct Node {
    T val = T();
    Node<T>* left = nullptr;
    Node<T>* right = nullptr;
    
    explicit Node(T val, Node<T>* left = nullptr, Node<T>* right = nullptr)
        : val{val}, left{left}, right{right} {}

    ~Node() {
        delete left;
        delete right;
    }
};
//make palindrome
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
    if (abs(left_height - right_height) > 1) return -1;
    return max(left_height, right_height) + 1;
}
bool is_balanced(Node<int>* tree) {
    return tree_height(tree) != -1;
}

// max depth
int dfs(Node<int>* root) {
    // Null node adds no depth
    if (root == nullptr) return 0;
    // num nodes in longest path of current subtree = max num nodes of its two subtrees + 1 current node
    return max(dfs(root->left), dfs(root->right)) + 1;
}
int tree_max_depth(Node<int>* root) {
    return root ? dfs(root) - 1 : 0;
}

// Visible tree nodes - max value is the current one than parents
int dfs(Node<int>* root, int max_sofar) {
    if (!root) return 0;
    int total = 0;
    if (root->val >= max_sofar) total++;
    total += dfs(root->left, max(max_sofar, root->val));
    total += dfs(root->right, max(max_sofar, root->val));
    return total;
}
int visible_tree_node(Node<int>* root) {
    // start max_sofar with smallest number possible so any value root has is greater than it
    return dfs(root, numeric_limits<int>::min());
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
// is valid BST
bool dfs(Node<int>* root, int min_val, int max_val) {
    if (!root) return true;
    if ((min_val >= root->val || root->val >= max_val)) return false;
    return dfs(root->left, min_val, root->val) && dfs(root->right, root->val, max_val);
}
bool valid_bst(Node<int>* root) {
    return dfs(root, numeric_limits<int>::min(), numeric_limits<int>::max());
}
// BST lowest common ancestor
int lca_on_bst(Node<int>* bst, int p, int q) {
    if (p < bst->val && q < bst->val) {
        return lca_on_bst(bst->left, p, q);
    } else if (p > bst->val && q > bst->val) {
        return lca_on_bst(bst->right, p, q);
    } else {
        return bst->val;
    }
}
// level order traversal
vector<vector<int>> level_order_traversal(Node<int>* root) {
    vector<vector<int>> res;
    if (root == nullptr) return res;
    queue<Node<int>*> level;
    level.push(root);
    while (!level.empty()) {
        int n = level.size();
        vector<int> new_level;
        for (int i = 0; i < n; i++) {
            Node<int>* node = level.front();
            new_level.emplace_back(node->val);
            if (node->left) level.push(node->left);
            if (node->right) level.push(node->right);
            level.pop();
        }
        res.emplace_back(new_level);
    }
    return res;
}

// level traversal zig zag
vector<vector<int>> zig_zag_traversal(Node<int>* root) {
    vector<vector<int>> res;
    if (root == nullptr) return res;
    queue<Node<int>*> level;
    level.push(root);
    bool left_to_right = true;
    while (!level.empty()) {
        int n = level.size();
        list<int> new_level;
        for (int i = 0; i < n; i++) {
            Node<int>* node = level.front();
            if (left_to_right) {
                new_level.emplace_back(node->val);
            } else {
                new_level.emplace_front(node->val);
            }
            if(node->left) level.push(node->left);
            if(node->right) level.push(node->right);
            level.pop();
        }
        res.emplace_back(new_level.begin(), new_level.end());
        left_to_right = !left_to_right; // flip flag
    }
    return res;
}
// right side view
vector<int> binary_tree_right_side_view(Node<int>* root) {
    vector<int> res;
    if (root == nullptr) return res;
    queue<Node<int>*> level;
    level.push(root);
    while (!level.empty()) {
        int n = level.size();
        // only append the first node we encounter since it's the rightmost
        res.emplace_back(level.front()->val);
        for (int i = 0; i < n; i++) {
            Node<int>* node = level.front();
            if (node->right) level.push(node->right);
            if (node->left) level.push(node->left);
            level.pop();
        }
    }
    return res;
}

// binary tree minimum depth
int binary_tree_min_depth(Node<int>* root) {
    queue<Node<int>*> level;
    int depth = -1; // start from -1 because popping root will add 1 depth
    level.push(root);
    while (!level.empty()) {
        depth++;
        int n = level.size();
        for (int i = 0; i < n; i++) {
            Node<int>* node = level.front();
            if (node->left == nullptr && node->right == nullptr) {
                return depth; // found leaf node, early return
            }
            if (node->left) level.push(node->left);
            if (node->right) level.push(node->right);
            level.pop();
        }
    }
    return depth;
}
// Shortest Path First
// BFS template
int bfs(std::vector<std::vector<int>>& graph, int root, int target) {
    std::queue<int> q;
    q.push(root);
    std::unordered_set<int> visited;
    visited.emplace(root);
    int level = 0;
    while (!q.empty()) {
        int n = q.size();
        for (int i = 0; i < n; i++) {
            int node = q.front();
            if (node == target) return level;
            for (int neighbor : graph[node]) {
                if (visited.count(neighbor)) continue;
                q.push(neighbor);
                visited.emplace(neighbor);
            }
            q.pop();
        }
        // increment level after we have processed all nodes of the level
        level++;
    }
    return level;
}

int shortest_path(std::vector<std::vector<int>>& graph, int a, int b) {
    return bfs(graph, a, b);
}

int main(int , char** )
{
    string test = "test";
    cout << makePalindrome(test) << endl;
}