#ifndef TABLE_HPP_
#define TABLE_HPP_

#include <iostream>

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

    Node(const T& source, Node* parent = nullptr) : left(nullptr), right(nullptr), parent(parent),  data(nullptr), color(RED)
    {
        data = new T(source);
    }

    Node(T*& source) : left(nullptr), right(nullptr), parent(nullptr), data(nullptr), color(RED)
    {
        data = source;
    }

    ~Node()
    {
        delete data;
        data = nullptr;

        delete left;
        delete right;
        left = nullptr;
        right = nullptr;
        parent = nullptr;
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

    Node*& _parent()
    {
        return parent;
    }

    T* _data()
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
    T* insert(const T& source, bool& kIsLeft)
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

    private:
    //The left child of this node
    Node* left;

    //The right child of this node
    Node* right;

    //This node's parent
    Node* parent;

    //The data this node manages
    T* data;

    //The color of this node (RED or BLACK)
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

    T* insert(const T& source)
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

        bool path[2] = {false, false};
        Node<T>* parent = nullptr;

        return insert(root, source, path, parent);
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

    bool isBalanced() const
    {
        if (!root) return true;
        int leftHeight = height(root->_left());
        int rightHeight = height(root->_right());
        std::cout << "L HEIGHT : " << leftHeight << "\nR HEIGHT : " << rightHeight << "\n\n";
        int difference = (leftHeight - rightHeight);
        if (difference < 0) difference *= -1;

        return difference <= 1;
    }

    private:
    Node<T>* root;
    size_t nodeCount;

    //Traverse with |root| to the null leaf where |source| belongs
    //Allocate and set the new node's |parent|
    //Make the necessary mutations to the tree to remain balanced

    //|path| contains information for |root| to grand child
    //true = left : false = right
    //index 0 : child path
    //index 1 : grandchild path

    //Return a pointer to the new data
    T* insert(Node<T>*& root, const T& source, bool path[2], Node<T>*& parent)
    {
        //At the null leaf
        if (!root)
        {
            //Allocate the new node with |source|
            root = new Node<T>(source);
            return root->_data();
        }

        //The pointer to return
        T* sourceptr = nullptr;

        //If |source| is less than root's data, go left
        if (root->lessThan(source))
        {
            sourceptr = insert(root->_left(), source, path, parent);

            //|root| is now the parent of |x| : set |parent| and |path|
            if (!parent)
            {
                path[0] = true;
                parent = root;
            }
            
            //|root| is now the grandparent of |x| : set |path|
            //Perform the necessary mutations
            else
            {
                path[1] = true;
                updateTree(root, path, parent);
            }
        }

        //|source is greater than or equal to root's data, go right
        else
        {
            sourceptr = insert(root->_right(), source, path, parent);

            //parent = 100 r
            //grandparent = 93 r

            //|root| is now the parent of |x| : set |parent| and |path|
            if (!parent)
            {
                path[0] = false;
                parent = root;
            }
            
            //|root| is now the grandparent of |x| : set |path|
            //Perform the necessary mutations
            else
            {
                path[1] = false;
                updateTree(root, path, parent);
            }
        }

        return sourceptr;
    }

    void updateTree(Node<T>*& grandparent, bool path[2], Node<T>*& parent)
    {
        //Find |x| based on the |path|
        Node<T>* x = (path[0] ? parent->_left() : parent->_right());

        //If the parent of |x| is black, nothing needs to be done
        if (!parent->isRed() || !x->isRed())
        {
            parent = nullptr;
            return;
        }

        //Find the uncle of |x| based of |path| to |parent|
        Node<T>* uncle = (path[1] ? grandparent->_right() : grandparent->_left());

        //If |uncle| of |x| is null or black
        if (!uncle || !uncle->isRed())
        {
            //path to |x| is left->left
            if (path[1] && path[0])
            {
                grandparent->recolor();
                parent->recolor();
                rotateRight(grandparent);
            }

            //path to |x| is right->left
            else if (!path[1] && path[0])
            {
                rotateRight(grandparent->_right());
                grandparent->recolor();
                grandparent->_right()->recolor();
                rotateLeft(grandparent);
            }

            //path to |x| is right->right
            else if (!path[1] && !path[0])
            {
                grandparent->recolor();
                parent->recolor();
                rotateLeft(grandparent);
            }

            //path to |x| is left->right
            else
            {
                rotateLeft(grandparent->_left());
                grandparent->recolor();
                grandparent->_left()->recolor();
                rotateRight(grandparent);
            }
        }

        //|uncle| of |x| is red
        else
        {
            parent->recolor();
            uncle->recolor();

            //Only recolor |grandparent| if it is not the true root
            if (grandparent != this->root) grandparent->recolor();
        }

        //|grandparent| is now the new |x|
        //The new parent must be found
        parent = nullptr;
    }

    static void rotateRight(Node<T>*& root)
    {
        //Hold onto the root
        Node<T>* newRight = root;

        //Push up the left child as the new root
        root = root->_left();

        //Adopt the new root's right subtree as the left subtree
        newRight->setLeft(root->_right());

        //Adopt the old root as the right child
        root->setRight(newRight);
    }

    static void rotateLeft(Node<T>*& root)
    {
        //Hold onto the root
        Node<T>* newLeft = root;

        //Push up the right child as the new root
        root = root->_right();

        //Adopt the new root's left subtree as the right subtree
        newLeft->setRight(root->_left());

        //Adopt the old root as the left child
        root->setLeft(newLeft);
    }

    //Display all elements in the tree in order
    void displayInorder(Node<T>* root, std::ostream& out = std::cout) const
    {
        if (!root) return;

        displayInorder(root->_left(), out);
        root->display(out);
        out << '\n';
        displayInorder(root->_right(), out);
    }

    //Display the level, data, and color of each node in the tree
    //Pre order traversal
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

    int height(Node<T>* root) const
    {
        if (!root) return 0;

        return 1 + std::max(height(root->_left()), height(root->_right()));
    }
};

#endif //TABLE_HPP_
