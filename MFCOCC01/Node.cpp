#include "Node.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Node::Node(const std::string& name, double w, double h)
    : name(name), x(0), y(0), w(w), h(h), isroot(false), used(false), right(nullptr), down(nullptr), fit(nullptr) {}

Node::Node(double x, double y, double w, double h)
    : x(x), y(y), w(w), h(h), name(""), isroot(false), used(false), right(nullptr), down(nullptr), fit(nullptr) {
    if (x == 0 && y == 0) {
        isroot = true;
    }
}