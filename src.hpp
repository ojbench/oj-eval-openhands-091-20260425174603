
#ifndef SRC_HPP
#define SRC_HPP

#include <iostream>
#include <memory>
#include <algorithm>
#include <stdexcept>

template<class Key, class Compare = std::less<Key>>
class ESet {
private:
    enum Color { RED, BLACK };
    
    struct Node {
        Key key;
        Color color;
        Node* left;
        Node* right;
        Node* parent;
        
        Node(const Key& k) : key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
        Node(Key&& k) : key(std::move(k)), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
        Node() : color(BLACK), left(nullptr), right(nullptr), parent(nullptr) {}
    };
    
    Node* root;
    Node* NIL;
    size_t sz;
    Compare comp;
    
    void init() {
        NIL = new Node();
        NIL->left = NIL->right = NIL->parent = NIL;
        root = NIL;
        sz = 0;
    }
    
    void destroy(Node* node) {
        if (node != NIL) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }
    
    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != NIL) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == NIL) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }
    
    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right != NIL) {
            x->right->parent = y;
        }
        x->parent = y->parent;
        if (y->parent == NIL) {
            root = x;
        } else if (y == y->parent->right) {
            y->parent->right = x;
        } else {
            y->parent->left = x;
        }
        x->right = y;
        y->parent = x;
    }
    
    void insertFixup(Node* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }
    
    void transplant(Node* u, Node* v) {
        if (u->parent == NIL) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }
    
    Node* minimum(Node* node) const {
        while (node->left != NIL) {
            node = node->left;
        }
        return node;
    }
    
    Node* maximum(Node* node) const {
        while (node->right != NIL) {
            node = node->right;
        }
        return node;
    }
    
    void eraseFixup(Node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                Node* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }
    
    Node* copyTree(Node* other, Node* otherNIL, Node* parent) {
        if (other == otherNIL) {
            return NIL;
        }
        Node* newNode = new Node(other->key);
        newNode->color = other->color;
        newNode->parent = parent;
        newNode->left = copyTree(other->left, otherNIL, newNode);
        newNode->right = copyTree(other->right, otherNIL, newNode);
        return newNode;
    }
    
    size_t countRange(Node* node, const Key& l, const Key& r) const {
        if (node == NIL) {
            return 0;
        }
        if (comp(node->key, l)) {
            return countRange(node->right, l, r);
        }
        if (comp(r, node->key)) {
            return countRange(node->left, l, r);
        }
        return 1 + countRange(node->left, l, r) + countRange(node->right, l, r);
    }
    
    size_t countRangeOptimized(const Key& l, const Key& r) const {
        size_t count = 0;
        Node* current = root;
        
        while (current != NIL) {
            if (comp(current->key, l)) {
                current = current->right;
            } else if (comp(r, current->key)) {
                current = current->left;
            } else {
                count++;
                if (current->left != NIL) {
                    Node* left = current->left;
                    while (left != NIL) {
                        if (!comp(left->key, l)) {
                            if (!comp(r, left->key)) {
                                count++;
                            }
                            left = left->left;
                        } else {
                            left = left->right;
                        }
                    }
                }
                if (current->right != NIL) {
                    Node* right = current->right;
                    while (right != NIL) {
                        if (!comp(right->key, l)) {
                            if (!comp(r, right->key)) {
                                count++;
                            }
                            right = right->left;
                        } else {
                            right = right->right;
                        }
                    }
                }
                break;
            }
        }
        
        return count;
    }
    
public:
    class iterator {
    private:
        Node* node;
        Node* NIL;
        const ESet* set;
        
    public:
        iterator() : node(nullptr), NIL(nullptr), set(nullptr) {}
        iterator(Node* n, Node* nilNode, const ESet* s) : node(n), NIL(nilNode), set(s) {}
        
        const Key& operator*() const {
            if (node == NIL || node == nullptr) {
                throw std::out_of_range("Iterator dereferenced end()");
            }
            return node->key;
        }
        
        iterator& operator++() {
            if (node == NIL || node == nullptr) {
                return *this;
            }
            if (node->right != NIL) {
                node = set->minimum(node->right);
            } else {
                Node* p = node->parent;
                while (p != NIL && node == p->right) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }
        
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        iterator& operator--() {
            if (node == NIL || node == nullptr) {
                if (set && set->root != set->NIL) {
                    node = set->maximum(set->root);
                }
                return *this;
            }
            if (set && node == set->minimum(set->root)) {
                return *this;
            }
            if (node->left != NIL) {
                node = set->maximum(node->left);
            } else {
                Node* p = node->parent;
                while (p != NIL && node == p->left) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }
        
        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const {
            return node == other.node;
        }
        
        bool operator!=(const iterator& other) const {
            return node != other.node;
        }
        
        friend class ESet;
    };
    
    ESet() {
        init();
    }
    
    ~ESet() {
        destroy(root);
        delete NIL;
    }
    
    ESet(const ESet& other) {
        init();
        sz = other.sz;
        root = copyTree(other.root, other.NIL, NIL);
    }
    
    ESet& operator=(const ESet& other) {
        if (this != &other) {
            destroy(root);
            sz = other.sz;
            root = copyTree(other.root, other.NIL, NIL);
        }
        return *this;
    }
    
    ESet(ESet&& other) noexcept {
        root = other.root;
        NIL = other.NIL;
        sz = other.sz;
        comp = std::move(other.comp);
        
        other.root = other.NIL = nullptr;
        other.sz = 0;
    }
    
    ESet& operator=(ESet&& other) noexcept {
        if (this != &other) {
            destroy(root);
            delete NIL;
            
            root = other.root;
            NIL = other.NIL;
            sz = other.sz;
            comp = std::move(other.comp);
            
            other.root = other.NIL = nullptr;
            other.sz = 0;
        }
        return *this;
    }
    
    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        Key key(std::forward<Args>(args)...);
        
        Node* y = NIL;
        Node* x = root;
        
        while (x != NIL) {
            y = x;
            if (comp(key, x->key)) {
                x = x->left;
            } else if (comp(x->key, key)) {
                x = x->right;
            } else {
                return std::make_pair(iterator(x, NIL, this), false);
            }
        }
        
        Node* z = new Node(std::move(key));
        z->parent = y;
        
        if (y == NIL) {
            root = z;
        } else if (comp(z->key, y->key)) {
            y->left = z;
        } else {
            y->right = z;
        }
        
        z->left = NIL;
        z->right = NIL;
        z->color = RED;
        
        insertFixup(z);
        sz++;
        
        return std::make_pair(iterator(z, NIL, this), true);
    }
    
    size_t erase(const Key& key) {
        Node* z = root;
        while (z != NIL) {
            if (comp(key, z->key)) {
                z = z->left;
            } else if (comp(z->key, key)) {
                z = z->right;
            } else {
                break;
            }
        }
        
        if (z == NIL) {
            return 0;
        }
        
        Node* y = z;
        Color yOriginalColor = y->color;
        Node* x;
        
        if (z->left == NIL) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == NIL) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        
        delete z;
        
        if (yOriginalColor == BLACK) {
            eraseFixup(x);
        }
        
        sz--;
        return 1;
    }
    
    iterator find(const Key& key) const {
        Node* node = root;
        while (node != NIL) {
            if (comp(key, node->key)) {
                node = node->left;
            } else if (comp(node->key, key)) {
                node = node->right;
            } else {
                return iterator(node, NIL, this);
            }
        }
        return end();
    }
    
    size_t range(const Key& l, const Key& r) const {
        if (comp(r, l)) {
            return 0;
        }
        return countRange(root, l, r);
    }
    
    size_t size() const noexcept {
        return sz;
    }
    
    iterator lower_bound(const Key& key) const {
        Node* node = root;
        Node* result = NIL;
        
        while (node != NIL) {
            if (!comp(node->key, key)) {
                result = node;
                node = node->left;
            } else {
                node = node->right;
            }
        }
        
        return iterator(result, NIL, this);
    }
    
    iterator upper_bound(const Key& key) const {
        Node* node = root;
        Node* result = NIL;
        
        while (node != NIL) {
            if (comp(key, node->key)) {
                result = node;
                node = node->left;
            } else {
                node = node->right;
            }
        }
        
        return iterator(result, NIL, this);
    }
    
    iterator begin() const noexcept {
        if (root == NIL) {
            return end();
        }
        return iterator(minimum(root), NIL, this);
    }
    
    iterator end() const noexcept {
        return iterator(NIL, NIL, this);
    }
};

#endif
