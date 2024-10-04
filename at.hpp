#ifndef AT_HPP
#define AT_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>

namespace AT {

// Helper struct for type punning
struct TreeNodeRaw {
    int val;
    TreeNodeRaw* left;
    TreeNodeRaw* right;
};

struct AsciiNode {
    std::unique_ptr<AsciiNode> left, right;
    int edge_length = 0;
    int height = 0;
    std::string label;
    int parent_dir = 0; // -1 = left, 0 = root, 1 = right

    AsciiNode(int value) : label(std::to_string(value)) {}
    
    int lablen() const { return static_cast<int>(label.length()); }
};

class AsciiTreePrinter {
public:
    AsciiTreePrinter(int gap_size = 3)
        : gap(gap_size),
          lprofile(MAX_HEIGHT, std::numeric_limits<int>::max()),
          rprofile(MAX_HEIGHT, std::numeric_limits<int>::min()) {}

    void print(void* node) {
        if (!node) return;

        auto root = reinterpret_cast<TreeNodeRaw*>(node);
        auto proot = build_ascii_tree(root);
        compute_edge_lengths(proot.get());

        std::fill(lprofile.begin(), lprofile.end(), std::numeric_limits<int>::max());
        compute_lprofile(proot.get(), 0, 0);

        int xmin = 0;
        for (int i = 0; i < proot->height && i < MAX_HEIGHT; ++i)
            xmin = std::min(xmin, lprofile[i]);

        for (int i = 0; i < proot->height; ++i) {
            print_next = 0;
            print_level(proot.get(), -xmin, i);
            std::cout << '\n';
        }

        if (proot->height >= MAX_HEIGHT)
            std::cout << "(This tree is taller than " << MAX_HEIGHT << ", and may be drawn incorrectly.)\n";
    }

private:
    static constexpr int MAX_HEIGHT = 1000;
    int gap;
    int print_next = 0;
    std::vector<int> lprofile;
    std::vector<int> rprofile;

    std::unique_ptr<AsciiNode> build_ascii_tree(TreeNodeRaw* t) const {
        if (!t) return nullptr;

        auto node = std::make_unique<AsciiNode>(t->val);
        node->left = build_ascii_tree(t->left);
        node->right = build_ascii_tree(t->right);

        if (node->left) node->left->parent_dir = -1;
        if (node->right) node->right->parent_dir = 1;

        return node;
    }

    void compute_lprofile(const AsciiNode* node, int x, int y) {
        if (!node) return;

        bool isLeft = (node->parent_dir == -1);
        lprofile[y] = std::min(lprofile[y], x - ((node->lablen() - isLeft) / 2));

        if (node->left) {
            for (int i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; ++i)
                lprofile[y + i] = std::min(lprofile[y + i], x - i);
        }

        compute_lprofile(node->left.get(), x - node->edge_length - 1, y + node->edge_length + 1);
        compute_lprofile(node->right.get(), x + node->edge_length + 1, y + node->edge_length + 1);
    }

    void compute_rprofile(const AsciiNode* node, int x, int y) {
        if (!node) return;

        bool notLeft = (node->parent_dir != -1);
        rprofile[y] = std::max(rprofile[y], x + ((node->lablen() - notLeft) / 2));

        if (node->right) {
            for (int i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; ++i)
                rprofile[y + i] = std::max(rprofile[y + i], x + i);
        }

        compute_rprofile(node->left.get(), x - node->edge_length - 1, y + node->edge_length + 1);
        compute_rprofile(node->right.get(), x + node->edge_length + 1, y + node->edge_length + 1);
    }

    void compute_edge_lengths(AsciiNode* node) {
        if (!node) return;

        compute_edge_lengths(node->left.get());
        compute_edge_lengths(node->right.get());

        if (!node->left && !node->right) {
            node->edge_length = 0;
        } else {
            if (node->left) compute_rprofile(node->left.get(), 0, 0);
            if (node->right) compute_lprofile(node->right.get(), 0, 0);

            int delta = 4;
            int hmin = 0;
            if (node->left && node->right)
                hmin = std::min(node->left->height, node->right->height);
            else if (node->left)
                hmin = node->left->height;
            else if (node->right)
                hmin = node->right->height;

            for (int i = 0; i < hmin; ++i)
                delta = std::max(delta, gap + 1 + rprofile[i] - lprofile[i]);

            if (((node->left && node->left->height == 1) ||
                 (node->right && node->right->height == 1)) && delta > 4) {
                --delta;
            }

            node->edge_length = ((delta + 1) / 2) - 1;
        }

        node->height = 1;
        if (node->left)
            node->height = std::max(node->height, node->left->height + node->edge_length + 1);
        if (node->right)
            node->height = std::max(node->height, node->right->height + node->edge_length + 1);
    }

    void print_level(const AsciiNode* node, int x, int level) {
        if (!node) return;

        if (level == 0) {
            std::cout << std::string(x - print_next - (node->lablen() / 2), ' ');
            print_next += x - print_next - (node->lablen() / 2);
            std::cout << node->label;
            print_next += node->lablen();
        } else if (node->edge_length >= level) {
            if (node->left) {
                std::cout << std::string(x - print_next - level, ' ') << '/';
                print_next += x - print_next - level + 1;
            }
            if (node->right) {
                std::cout << std::string(x - print_next + level, ' ') << '\\';
                print_next += x - print_next + level + 1;
            }
        } else {
            print_level(node->left.get(), x - node->edge_length - 1, level - node->edge_length - 1);
            print_level(node->right.get(), x + node->edge_length + 1, level - node->edge_length - 1);
        }
    }
};

inline void print_ascii_tree(void* node) {
    AsciiTreePrinter printer;
    printer.print(node);
}

} // namespace AT

#endif /* AT_HPP */

/*

// Example code

#include <iostream>
#include <memory>
#include "at.hpp"

class Node {
public:
    int v;
    std::unique_ptr<Node> l, r;
    explicit Node(int value) : v(value), l(nullptr), r(nullptr) {}
};

class BST {
public:
    std::unique_ptr<Node> root;
    bool add(int v) {
        return add(root, v);
    }
    void release() {
        root.reset();  // Automatically releases memory
    }
    Node* get_root() const {
        return root.get();  // Return raw pointer to the root node
    }
private:
    bool add(std::unique_ptr<Node>& node, int v) {
        if (!node) {
            node = std::make_unique<Node>(v);
            return true;
        }
        if (v < node->v) {
            return add(node->l, v);
        } else if (v > node->v) {
            return add(node->r, v);
        }
        return false;  // Value already exists, no duplicates
    }
};

int main() {
    BST bst;
    for (int n : {3, 6, 1, 2, 9, 4, 0, 5}) {
        bst.add(n);
    }
    // Pass a raw pointer to the root node
    AT::print_ascii_tree(static_cast<void*>(bst.get_root()));
    bst.release();
    return 0;
}


*/
