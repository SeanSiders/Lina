/*
This data structure is a Red-Black Tree, which is used in this program to store the matrices in dynamic memory. This
program uses an external file to store matrices and operations upon exiting the program. The tree will read in and write
out in pre-order traversal. The underlying data is responsible for writing out and reading in its own data.

READING IN FROM THE DATAFILE
A parameterized constructor needs to take in a std::ifstream& where data can then be read in from the abstraction.

WRITING OUT FROM THE DATAFILE
The std::ofstream& operator<< needs to be overloaded for underlying data to write data out to the datafile.

RED BLACK TREE PROPERTIES
- The root is always black
- All null children are considered black
- Children of red nodes are always black
- Every simple downard path to a leaf contains the same number of black nodes
- The longest path to a leaf is no more than twice the length of the shortest path to a leaf
- Complexity is O(log N)

REQUIRED OPERATOR OVERLOADS
bool operator< | bool operator> : for sorting operations
std::ostream& operator<< : for displaying from this tree
std::ofstream& opeartor<< : for writing out to the external datafile

@Sean Siders
sean.siders@icloud.com
*/

#ifndef TABLE_HPP_
#define TABLE_HPP_

#include <iostream>
#include <fstream>

//// FORWARD DECLARATIONS

template <typename T>
class Node;

template <typename T>
class Tree;

//// GLOBAL OVERLOAD 

template <typename T>
std::ostream& operator<<(std::ostream&, const Tree<T>&);

//////// NODE COLOR BIT

//Every node in the tree contains a |ColorBit| which is either RED or BLACK
enum ColorBit
{
    RED,
    BLACK
};

//////// RED BLACK NODE

template <typename T>
class Node
{
    public:

    //////// CONSTRUCTORS 

    Node() : left(nullptr), right(nullptr), data(nullptr), color(RED) {}

    //Parameterized to take in the |source| data that this node will manage
    Node(const T& source) : left(nullptr), right(nullptr), data(nullptr), color(RED)
    {
        data = new T(source);
    }

    Node(std::ifstream& inFile) : left(nullptr), right(nullptr), data(nullptr)
    {
        data = new T(inFile);
        inFile.ignore(1, '\n');
    }

    //////// DESTRUCTOR 

    //Recursive tree deallocation
    ~Node()
    {
        delete data;
        data = nullptr;

        delete left;
        delete right;
        left = nullptr;
        right = nullptr;
    }

    //////// PUBLIC FUNCTIONS 

    //True if |other| is less than this node's data
    template <typename K = T>
    bool lessThan(const K& other) const
    {
        return (data ? *data > other : false);
    }

    //True if |other| is greater than this node's data
    template <typename K = T>
    bool greaterThan(const K& other) const
    {
        return (data ? *data < other : false);
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

    //Display the data of this node using |out|
    void display(std::ostream& out = std::cout) const
    {
        if (data) out << *data;
    }

    //Display the color of this node (for debugging purposes)
    void debugDisplayColor() const
    {
        std::cout << (color == RED ? "RED" : "BLK");
    }

    //Write the data in this node to |outFile|
    void writeFile(std::ofstream& outFile) const
    {
        if (data) outFile << *data;

        outFile << '\n' << (left ? '1' : '0') << ' '
        << (right ? '1' : '0') << '\n';
    }

    //////// GETTERS / SETTERS 

    Node*& _left()
    {
        return left;
    }

    Node*& _right()
    {
        return right;
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

    private:

    //////// DATA 

    //The left child of this node
    Node* left;

    //The right child of this node
    Node* right;

    //The data this node manages (dynamically allocated)
    T* data;

    //The color of this node (RED or BLACK)
    ColorBit color;
};

template <typename T>
class Tree 
{
    public:

    //////// OPERATOR OVERLOAD 

    //Display the tree in order from smallest key to largest key
    friend std::ostream& operator<<(std::ostream& out, const Tree& tree)
    {
        tree.displayInorder(out);
        return out;
    }

    //////// CONSTRUCTOR

    Tree() : root(nullptr), nodeCount(0), filename(nullptr) {}

    //A |_filename| to an external database was provided
    //Read in the data from the file, populating the tree
    Tree(const char* _filename) : root(nullptr), nodeCount(0), filename(_filename)
    {
        std::ifstream inFile(filename);
        if (inFile)
        {
            inFile >> nodeCount;

            if (inFile.eof()) nodeCount = 0;
            else readFile(root, inFile);

            inFile.clear();
            inFile.close();
        }
    }

    //////// DESTRUCTOR

    ~Tree()
    {
        //If an external file was provided
        //Write out to the file and recursively deallocate the tree
        if (filename)
        {
            std::ofstream outFile(filename);
            
            if (root)
            {
                outFile << nodeCount << '\n';
                writeFile(root, outFile);
            }

            outFile.clear();
            outFile.close();
        }

        //Otherwise just deallocate
        //Recursive dellocation occurs in the Node destructor
        delete root;
        root = nullptr;
    }

    //////// PUBLIC FUNCTIONS 

    //Insert |source| into the tree
    //Various mutations occur in the recursive call to maintain red-black tree properties
    //Return a pointer to the inserted data
    T* insert(const T& source)
    {
        //Increment node count
        ++nodeCount;

        //Empty Tree
        if (!root)
        {
            //Allocate and make root black
            root = new Node<T>(source);
            root->recolor();
            return root->_data();
        }

        bool path[2] = {false, false};
        Node<T>* parent = nullptr;

        return insert(root, source, path, parent);
    }

    template <typename K = T>
    T* retrieve(const K& key) const
    {
        return retrieve(root, key);
    }

    //Return the number of nodes / items in the tree
    size_t size() const
    {
        return nodeCount;
    }

    void displayInorder(std::ostream& out = std::cout) const
    {
        displayInorder(root, out);
    }

    //Display in preorder traversal showing the level, data, and color of each node
    void debugDisplay() const
    {
        if (!root) return;

        std::cout << "ROOT" << " : " << *root->_data() << " ";
        root->debugDisplayColor();
        std::cout << '\n';

        debugDisplay(root);
    }

    private:

    //////// DATA 

    //The root of the tree
    Node<T>* root;

    //The number of nodes / items in this tree
    size_t nodeCount;

    //The filename of the external database if one was provided
    //The data structure will read in / write out to the specified file
    const char* filename;

    //////// PRIVATE FUNCTIONS 

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

        //If |source| key is less than root's key, go left
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

        //|source| key is greater than root's key, go right
        else
        {
            sourceptr = insert(root->_right(), source, path, parent);

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

    //|grandparent| : the current |root| in insert
    //|path| : the path from |grandparent| to |x| : true = left / false = right
    //|parent| the child of grandparent on the path to |x|
    //This function is called every 3rd node in head recursion of the insert procedure
    //Various cases are checked to evaluate whether the tree is still a proper red-black tree
    //If not, the necessary mutations
    void updateTree(Node<T>*& grandparent, bool path[2], Node<T>*& parent)
    {
        //Find |x| based on the provided |path|
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

    //Rotate |root| to the right
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

    //Rotate |root| to the left
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

    template <typename K = T>
    static T* retrieve(Node<T>* root, const K& key)
    {
        if (!root) return nullptr;

        //|key| is less than root's key
        if (root->lessThan(key)) return retrieve(root->_left(), key);

        //|key| is greater than root's key
        else if (root->greaterThan(key)) return retrieve(root->_right(), key);

        //|key| is equal to root's key
        //Data to retrieve was found
        return root->_data();
    }

    //Traverse the tree with |root| in order, displaying all items
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
    void debugDisplay(Node<T>* root, size_t level = 1) const
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

        debugDisplay(root->_left(), level + 1);
        debugDisplay(root->_right(), level + 1);
    }

    //Read in data from |inFile| which was sequentally saved from a previous run of this program
    //The node and matrix read in from this file as well
    static void readFile(Node<T>*& root, std::ifstream& inFile)
    {
        root = new Node<T>(inFile);

        int hasLeft, hasRight;
        inFile >> hasLeft;
        inFile >> hasRight;
        inFile.ignore(1, '\n');

        //Only traverse if there is at least 1 child
        if (hasLeft) readFile(root->_left(), inFile);
        if (hasRight) readFile(root->_right(), inFile);
    }

    //Write all data into |outFile|
    //Deallocate the entire tree after
    //The node will write out the underlying data to the file using the overloaded operator <<
    static void writeFile(Node<T>*& root, std::ofstream& outFile)
    {
        if (!root) return;

        root->writeFile(outFile);

        writeFile(root->_left(), outFile);
        writeFile(root->_right(), outFile);

        delete root;
        root = nullptr;
    }
};

#endif //TABLE_HPP_
