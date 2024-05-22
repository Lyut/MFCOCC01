#pragma once
#ifndef PACKER_H
#define PACKER_H

#include <vector>
#include "Node.h"

class Packer {
private:
    Node root;
    bool used;

public:
    Packer(double w, double h);
    void fit(std::vector<Node>& blocks);
    Node* findNode(Node* root, double w, double h);
    Node* splitNode(Node* node, double w, double h);
};

#endif // PACKER_H
