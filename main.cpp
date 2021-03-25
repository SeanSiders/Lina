#include <iostream>
#include "Interface.hpp"

int main()
{
    /*
    std::cout << "\n\nLina -- (Linear Algebra Calculator)\n\n";

    Interface interface;
    bool running = true;

    do
    {
        running = interface.run();

    } while (running);
    */

    Tree<int> tree;

    tree.insert(61);
    tree.insert(52);
    tree.insert(85);
    tree.insert(76);
    tree.insert(93);

    tree.insert(100);

    tree.levelDisplay();

    return 0;
}
