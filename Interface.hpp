/*
This file manages all operations that involve input and output from the user for the linear algebra calculator. The
Interface class constructor is parameterized to take in a filename to allow for defined matrices and operations to be
stored in a file upon exiting the program. The Interface works directly with |Tree.hpp| to define and store matrices in
a data structure in the form of a Red-Black Tree.

@Sean Siders
sean.siders@icloud.com
*/

#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include <iostream>
#include <sstream>
#include "Matrix.hpp"
#include "Tree.hpp"

//This enum is used to efficiently branch the program to different processes
enum Commands
{
    INVALID_CMD, //The command was invalid
    DEFINE, //The user wants to define a new variable
    DISPLAY, //The user wants to display 1 or more matrices
    CLEAR, //The user wants to clear the screen
    OPERATE, //The user wants to apply operations on the matrices
    QUIT //Terminate the program
};

//This enum will be used to perform various operations on the matrices
enum Operators
{
    INVALID_OP,
    PLUS,
    MINUS,
    MULTIPLY
};

class Interface
{
    public:
    Interface();
    Interface(const char* filename);

    //Prompt the user for input
    //Branch to different parts of the program based on the input
    //Return false only when user enters 'q' or "quit", to end the program
    bool run();

    private:
    //The data structure that holds all defined matrices by their keys
    Tree<Matrix> matrixTree;

    //The most recent matrix that has been referenced by the user
    //Used for direct access, avoiding the need for retrieval from |matrixTree|
    Matrix* recent;

    //Determine which command the user entered, return the respective |Commands|
    static Commands evaluateCommand(const std::string& command);

    //Prompt the user that their |command| was not valid
    static void invalidCommand(const std::string& command);

    //Count the number of |columns| and |rows| in |matrixString|
    //Check that each entrie contains numeric expressions
    //Return false if any rows do not match the 1st row's column count, or an entrie is non-numeric
    static bool validMatrixInput(std::string& matrixString, size_t& rows, size_t& columns);

    //Return true only if |c| contains a char that is in the pool of valid char input for a matrix entry
    //VALID ENTRY CHARS
    //Any digit or '-' '.'
    static bool validChar(const char c);

    //Define a matrix as a particular key, insert it into the database
    void define(std::istringstream& stream);

    //Either display all matrices or specified matrices by key as additional arguments in the |stream|
    void display(std::istringstream& stream) const;

    //Print 100 newline characters
    void clearScreen() const;

    //Attempt operations on defined matrices based on user input
    //If the input is valid, display the resulting calculation
    //This algorithm will recursively continue until either invalid input occurs or the end of the stream is reached
    //|result| will contain the resulting matrix if successful
    bool operate(std::istringstream& stream, Matrix*& result) const;

    //Evaluate which operator the user input, then return the cooresponding enum
    Operators evaluateOperator(const std::string& operatorString) const;

    //Attempt to add two matrices |lhs| and |rhs|
    //If |positive| is false, |rhs| will be subracted from |lhs|
    //If the matries are not of the same order, addition cannot occur, return false
    bool add(const Matrix* lhs, const Matrix* rhs, const bool positive, Matrix*& result) const;

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
