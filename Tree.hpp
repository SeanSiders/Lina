#ifndef TABLE_HPP_
#define TABLE_HPP_

#include <iostream>
#include <memory>

template <typename T>
class Node;

template <typename T>
class Tree;

template <typename T>
std::ostream& operator<<(std::ostream&, const Tree<T>&);

template <typename T>
std::ostream& operator<<(std::ostream&, const Node<T>&);

enum RedBlack
{
    RED,
    BLACK
};

template <typename T>
class Node
{
    public:
    friend std::ostream& operator<<(std::ostream& out, const Node& node)
    {
        node.display(out);
        return out;
    }

    Node() : left(nullptr), right(nullptr), parent(nullptr), data(nullptr), color(RED) {}

    ~Node()
    {
        data = nullptr;
        delete left;
        delete right;
        left = nullptr;
        right = nullptr;
        parent = nullptr;
    }

    Node(const T& source) : left(nullptr), right(nullptr), parent(nullptr), data(nullptr), color(RED)
    {
        data = std::make_shared<T>(source);
    }

    Node(std::shared_ptr<T>& source) : left(nullptr), right(nullptr), parent(nullptr), data(nullptr), color(RED)
    {
        data = source;
    }

    //True if |other| is less than this node's data
    bool lessThan(const T& other) const
    {
        return (data ? other < *data : false);
    }

    //True if |other| is greater than this node's data
    bool greaterThan(const T& other) const
    {
        return (data ? other > *data : false);
    }

    //True if this node's data is equal to |other|
    bool equal(const T& other) const
    {
        return (data ? *data == other : false);
    }

    //True if this node has no children
    bool isLeaf() const
    {
        return (!left && !right);
    }

    //True if this node is red
    bool isRed() const
    {
        return RED == color;
    }

    //Change the color of this node
    void recolor()
    {
        color = (color == RED ? BLACK : RED);
    }

    void display(std::ostream& out = std::cout) const
    {
        if (data) out << *data;
    }

    Node*& _left()
    {
        return left;
    }

    Node*& _right()
    {
        return right;
    }

    std::shared_ptr<T> _data()
    {
        return data;
    }

    void setLeft(Node* _left)
    {
        left = _left;
    }

    void setRight(Node* _right)
    {
        right = _right;
    }

    void debugDisplayColor() const
    {
        std::cout << (color == RED ? "RED" : "BLK");
    }

    //Determine whether |source| belongs as a left child or right child
    //kIsLeft is true if |source| is the left child
    //Allocate and return a shared pointer to the data
    std::shared_ptr<T> insert(const T& source, bool& kIsLeft)
    {
        if (source < *data)
        {
            kIsLeft = true;
            left = new Node(source);

            return left->_data();
        }

        kIsLeft = false;
        right = new Node(source);

        return right->_data();
    }

    //Assign children's parent pointers to this node
    void update()
    {
        if (left)
        {
            left->parent = this;
        }

        if (right)
        {
            right->parent = this;
        }
    }

    //private:
    Node* left;
    Node* right;
    Node* parent;
    std::shared_ptr<T> data;
    RedBlack color;
};

template <typename T>
class Tree 
{
    public:
    friend std::ostream& operator<<(std::ostream& out, const Tree& tree)
    {
        tree.displayInorder(out);
        return out;
    }

    Tree() : root(nullptr), nodeCount(0) {}

    ~Tree()
    {
        delete root;
        root = nullptr;
    }

    std::shared_ptr<T> insert(const T& source)
    {
        //1) Empty Tree
        if (!root)
        {
            root = new Node<T>(source);

            //Make root black
            root->recolor();
            ++nodeCount;
            return root->_data();
        }

        bool kIsLeft = false;
        bool updated = false;
        return insert(root, source, kIsLeft, updated);
    }

    void displayInorder(std::ostream& out = std::cout) const
    {
        displayInorder(root, out);
    }

    void levelDisplay() const
    {
        if (!root) return;

        std::cout << "ROOT" << " : " << *root->_data() << " ";
        root->debugDisplayColor();
        std::cout << '\n';

        levelDisplay(root);
    }

    private:
    Node<T>* root;
    size_t nodeCount;

    //Insert |source| at the leaf
    //Recolor and rotate nodes as needed to keep the tree balanced
    std::shared_ptr<T> insert(Node<T>*& root, const T& source, bool& kIsLeft, bool& updated)
    {
        //Insert from the parent of |k|
        if (!root) return nullptr;
        
        std::shared_ptr<T> sourcePtr;

        //|source| belongs in the left subtree
        if (root->lessThan(source))
        {
            if (!root->_left())
            {
                kIsLeft = true;
                root->_left() = new Node<T>(source);

                return root->_left()->_data();
            }

            else sourcePtr = insert(root->_left(), source, kIsLeft, updated);

            if (!updated)
            {
                Node<T>* p = root->_left();
                Node<T>* k = (kIsLeft ? p->_left() : p->_right());
                updateTree(k, p, root, kIsLeft, true);
                updated = true;
            }
        }

        //|source| belongs in the right subtree
        else
        {
            if (!root->_right())
            {
                kIsLeft = false;
                root->_right() = new Node<T>(source);

                return root->_right()->_data();
            }

            else sourcePtr = insert(root->_right(), source, kIsLeft, updated);

            if (!updated)
            {
                Node<T>* p = root->_right();
                Node<T>* k = (kIsLeft ? p->_left() : p->_right());
                updateTree(k, p, root, kIsLeft, false);
                updated = true;
            }
        }

        return sourcePtr;
    }

    //k : the newly inserted node
    //p : k's parent
    //g : k's grandparent & p's parent
    void updateTree(Node<T>*& k, Node<T>*& p, Node<T>*& g, const bool kIsLeft, const bool pIsLeft)
    {
        //2) Parent is black, or the true root
        if (!p->isRed() || !g) return;

        //The uncle of |k|
        Node<T>* u;

        if (pIsLeft) u = g->_right();
        else u = g->_left();

        /*
        std::cout << "k : " << *k->_data() << (kIsLeft ? " is left\n" : " is right\n")
        << "p : " << *p->_data() << (pIsLeft ? " is left\n" : " is right\n")
        << "g : " << *g->_data() << '\n';
        if (u) std::cout << "u : " << *u->_data() << "\n\n";

        std::cin.get();
        */

        //Uncle is null or black
        if (!u || !u->isRed())
        {
            if (!kIsLeft && !pIsLeft)
            {
                g->recolor();
                p->recolor();
                rotateLeft(g);
            }
            else if (kIsLeft && !pIsLeft)
            {
                rotateRight(g->_right());
                g->recolor();
                k->recolor();
                rotateLeft(g);
            }
            else if (kIsLeft && pIsLeft)
            {
                g->recolor();
                p->recolor();
                rotateRight(g);
            }
            else
            {
                rotateLeft(g->_left());
                g->recolor();
                k->recolor();
                rotateRight(g);
            }
        }

        //Uncle is red
        else
        {
            p->recolor();
            u->recolor();
            if (g != this->root) g->recolor();
        }
    }

    void rotateRight(Node<T>*& root)
    {
        Node<T>* hold = root;
        root = root->_left();
        hold->setLeft(nullptr);
        hold->setRight(root->_right());
        root->setRight(hold);
    }

    void rotateLeft(Node<T>*& root)
    {
        Node<T>* hold = root;
        root = root->_right();
        hold->setRight(nullptr);
        hold->setRight(root->_left());
        root->setLeft(hold);
    }

    void displayInorder(Node<T>* root, std::ostream& out = std::cout) const
    {
        if (!root) return;

        displayInorder(root->_left(), out);
        root->display(out);
        out << '\n';
        displayInorder(root->_right(), out);
    }

    void levelDisplay(Node<T>* root, size_t level = 1) const
    {
        if (!root) return;

        if (root->_left() && root->_right())
        {
            std::cout << "LEVEL " << level << " : "
            << *root->_left()->_data() << " ";
            root->_left()->debugDisplayColor();

            std::cout << "   " << *root->_right()->_data() << " ";
            root->_right()->debugDisplayColor();

            std::cout << '\n';
        }
        else if (root->_left())
        {
            std::cout << "LEVEL " << level << " : " << *root->_left()->_data() << " ";
            root->_left()->debugDisplayColor();
            std::cout << '\n';
        }
        else if (root->_right())
        {
            std::cout << "LEVEL " << level << " : " << *root->_right()->_data() << " ";
            root->_right()->debugDisplayColor();
            std::cout << '\n';
        }
        else return;

        levelDisplay(root->_left(), level + 1);
        levelDisplay(root->_right(), level + 1);
    }
};

#endif //TABLE_HPP_
