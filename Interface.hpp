#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include <iostream>
#include <sstream>
#include "Matrix.hpp"
#include "Tree.hpp"

//This enum is used to efficiently branch the program to different processes
enum Commands
{
    INVALID, //The command was invalid
    DEFINE, //The user wants to define a new variable
    QUIT //Terminate the program
};

class Interface
{
    public:
    Interface();

    //Prompt the user for input
    //Branch to different parts of the program based on the input
    //Return false only when user enters 'q' or "quit", to end the program
    bool run();

    private:
    //The data structure that holds all defined matrices by their keys
    //Tree<Matrix> matrixTree;

    //Determine which command the user entered, return the respective |Commands|
    static Commands evaluateCommand(const std::string& command);

    //Prompt the user that their |command| was not valid
    static void invalidCommand(const std::string& command);

    //Count the number of |columns| and |rows| in |matrixString|
    //Check that each entrie contains numeric expressions
    //Return false if any rows do not match the 1st row's column count, or an entrie is non-numeric
    static bool validMatrixInput(const std::string& matrixString, size_t& rows, size_t& columns);

    //Return true only if |c| contains a char that is in the pool of valid char input for a matrix entry
    //VALID ENTRIE CHARS
    //Any digit or '-' '.'
    static bool validChar(const char c);

    //Define a matrix as a particular key, insert it into the database
    void define(std::istringstream& stream);

    //Check if |key| is already bound to an existing matrix
    //If it is, ask whether the user wants to overwrite with a new matrix
    //If yes, return true
    bool overwriteCheck(std::string& key);

    //Get valid matrix input from the user
    //Return false if the user has invalid input, and they choose to quit
    bool getMatrixInput(const std::string& key, std::string& matrixString, size_t& rows, size_t& columns);

    //Get either a 'y' for "yes" or 'n' for "no"
    //If yes, return true
    bool getYesNo();
};

#endif //INTERFACE_HPP_
