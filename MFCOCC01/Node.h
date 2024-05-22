#pragma once
#ifndef NODE_H
#define NODE_H

#include <string>

class Node {
public:
    bool isroot;
    std::string name;
    double x, y, w, h;
    bool used;
    Node* right, * down, * fit;

    Node(const std::string& name, double w, double h);
    Node(double x, double y, double w, double h);
};

#endif // NODE_H
