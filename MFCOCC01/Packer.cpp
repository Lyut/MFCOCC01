#include "Packer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Packer::Packer(double w, double h) : root(0, 0, w, h), used(false) {}

void Packer::fit(std::vector<Node>& blocks) {
    Node* node;
    Node* block;
    auto blockItr = blocks.begin();
    while (blockItr != blocks.end()) {
        block = &(*blockItr);
        if ((node = findNode(&root, block->w, block->h)) != nullptr) {
            block->fit = splitNode(node, block->w, block->h);
        }
        else {
            // No space left in the bin
            // You may handle this case as per your requirement
        }
        ++blockItr;
    }
}

Node* Packer::findNode(Node* root, double w, double h) {
    if (root->used) {
        Node* right = findNode(root->right, w, h);
        return (right != nullptr ? right : findNode(root->down, w, h));
    }
    else if ((w <= root->w) && (h <= root->h)) {
        return root;
    }
    else {
        return nullptr;
    }
}

Node* Packer::splitNode(Node* node, double w, double h) {
    node->used = true;
    node->down = new Node(node->x, node->y + h, node->w, node->h - h);
    node->right = new Node(node->x + w, node->y, node->w - w, h);
    return node;
}