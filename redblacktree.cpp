#include <iostream>
#include <memory>
#include <vector>

using namespace std;

enum Color { RED, BLACK };

struct Node {
    double data;
    Color color;
    shared_ptr<Node> left, right, parent;

    Node(double data) : data(data), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class PersistentRBTree {
private:
    shared_ptr<Node> root;
    vector<shared_ptr<Node>> versions;

    void rotateLeft(shared_ptr<Node>& root, shared_ptr<Node>& pt) {
        shared_ptr<Node> pt_right = pt->right;
        pt->right = pt_right->left;
        if (pt->right != nullptr)
            pt->right->parent = pt;
        pt_right->parent = pt->parent;
        if (pt->parent == nullptr)
            root = pt_right;
        else if (pt == pt->parent->left)
            pt->parent->left = pt_right;
        else
            pt->parent->right = pt_right;
        pt_right->left = pt;
        pt->parent = pt_right;
    }

    void rotateRight(shared_ptr<Node>& root, shared_ptr<Node>& pt) {
        shared_ptr<Node> pt_left = pt->left;
        pt->left = pt_left->right;
        if (pt->left != nullptr)
            pt->left->parent = pt;
        pt_left->parent = pt->parent;
        if (pt->parent == nullptr)
            root = pt_left;
        else if (pt == pt->parent->left)
            pt->parent->left = pt_left;
        else
            pt->parent->right = pt_left;
        pt_left->right = pt;
        pt->parent = pt_left;
    }

    void fixViolation(shared_ptr<Node>& root, shared_ptr<Node>& pt) {
        shared_ptr<Node> parent_pt = nullptr;
        shared_ptr<Node> grand_parent_pt = nullptr;
        while ((pt != root) && (pt->color != BLACK) && (pt->parent->color == RED)) {
            parent_pt = pt->parent;
            grand_parent_pt = pt->parent->parent;
            if (parent_pt == grand_parent_pt->left) {
                shared_ptr<Node> uncle_pt = grand_parent_pt->right;
                if (uncle_pt != nullptr && uncle_pt->color == RED) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    pt = grand_parent_pt;
                } else {
                    if (pt == parent_pt->right) {
                        rotateLeft(root, parent_pt);
                        pt = parent_pt;
                        parent_pt = pt->parent;
                    }
                    rotateRight(root, grand_parent_pt);
                    swap(parent_pt->color, grand_parent_pt->color);
                    pt = parent_pt;
                }
            } else {
                shared_ptr<Node> uncle_pt = grand_parent_pt->left;
                if (uncle_pt != nullptr && uncle_pt->color == RED) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    pt = grand_parent_pt;
                } else {
                    if (pt == parent_pt->left) {
                        rotateRight(root, parent_pt);
                        pt = parent_pt;
                        parent_pt = pt->parent;
                    }
                    rotateLeft(root, grand_parent_pt);
                    swap(parent_pt->color, grand_parent_pt->color);
                    pt = parent_pt;
                }
            }
        }
        root->color = BLACK;
    }

    shared_ptr<Node> cloneNode(shared_ptr<Node> node) {
        if (!node) return nullptr;
        shared_ptr<Node> newNode = make_shared<Node>(node->data);
        newNode->color = node->color;
        newNode->left = node->left;
        newNode->right = node->right;
        newNode->parent = node->parent;
        return newNode;
    }

    shared_ptr<Node> insert(shared_ptr<Node> root, shared_ptr<Node> pt) {
        if (root == nullptr)
            return pt;
        if (pt->data < root->data) {
            shared_ptr<Node> leftChild = cloneNode(root->left);
            root->left = insert(leftChild, pt);
            root->left->parent = root;
        } else if (pt->data > root->data) {
            shared_ptr<Node> rightChild = cloneNode(root->right);
            root->right = insert(rightChild, pt);
            root->right->parent = root;
        }
        return root;
    }

public:
    PersistentRBTree() : root(nullptr) {}

    void insert(double data) {
        shared_ptr<Node> pt = make_shared<Node>(data);
        root = insert(root, pt);
        fixViolation(root, pt);
        versions.push_back(root);
    }

    void printInOrder(shared_ptr<Node> root) {
        if (root == nullptr)
            return;
        printInOrder(root->left);
        cout << root->data << " ";
        printInOrder(root->right);
    }

    void printCurrentVersion() {
        if (versions.empty()) {
            cout << "Tree is empty" << endl;
            return;
        }
        printInOrder(versions.back());
        cout << endl;
    }

    shared_ptr<Node> getRoot(int version) {
        if (version < 0 || version >= versions.size()) {
            return nullptr;
        }
        return versions[version];
    }
};

int main() {
    PersistentRBTree tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(15);

    cout << "Current version of the tree: ";
    tree.printCurrentVersion();

    cout << "Tree version 1: ";
    shared_ptr<Node> version1 = tree.getRoot(1);
    if (version1) tree.printInOrder(version1);
    cout << endl;

    cout << "Tree version 2: ";
    shared_ptr<Node> version2 = tree.getRoot(2);
    if (version2) tree.printInOrder(version2);
    cout << endl;

    return 0;
}
