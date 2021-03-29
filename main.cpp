#include <iostream>
#include "Interface.hpp"

//// GLOBAL CONSTANT
const char* FILENAME = "matrices.txt";

int main()
{
    std::cout << "\n\nLina -- (Linear Algebra Calculator)\n\n";

    Interface interface(FILENAME);
    bool running = true;

    do
    {
        running = interface.run();

    } while (running);

    return 0;
}
