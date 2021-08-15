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
#include "ExceptionHandler.hpp"

//This enum is used to efficiently branch the program to different processes
enum Commands
{
    DEFINE, //The user wants to define a new variable
    DISPLAY, //The user wants to display 1 or more matrices
    CLEAR, //The user wants to clear the screen
    OPERATE, //The user may be trying to apply 1 or more matrix operations
    HELP, //The user wants help on how to use Lina
    QUIT //Terminate the program
};

//This enum will be used to perform various operations on the matrices
enum Operators
{
    INVALID_OP, //Operater does not exist
    PLUS, //The user wants to add matrices
    MINUS, //The user wants to subtract matrices
    MULTIPLY, //The user wants to multiply matrices
    ASSIGN //The user wants to assign an identifier to the resulting matrix
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
    
    //Prompt the user with instructions on how to use Lina
    void helpPrompt() const;

    void operate(const std::string& lhsKey, std::istringstream& stream);

    //Returns the resulting matrix, used in |assign|
    void operate(const Matrix* lhs, const Operators op, const Matrix* rhs, Matrix*& result, const std::string& resultKey);

    //Evaluate which operator the user input, then return the cooresponding enum
    Operators evaluateOperator(const std::string& operatorString) const;

    //Attempt to add two matrices |lhs| and |rhs|
    //If |positive| is false, |rhs| will be subracted from |lhs|
    //If the matrices are not of the same order, addition cannot occur, an exception is thrown
    static void add(const Matrix* lhs, const Matrix* rhs, const bool positive);

    //Attempt to multiple two matrices |lhs| and |rhs|
    //If the columns of |lhs| do not match the rows of |rhs|, multiplication cannot occur, return false
    static void multiply(const Matrix* lhs, const Matrix* rhs);

    //Attempt to assign |result| as the resulting matrix from an operation that begins with |lhs|
    //Proceed to read in the operator, and the righ operand matrix
    //Will throw an exception if :
    //  - The operator is invalid 
    //  - The operator is another assignment (=)
    //  - The right operand identifier does not exist
    void assign(Matrix*& result, const std::string& resultKey, const Matrix* lhs, std::istringstream& stream);

    //Check if |key| is already bound to an existing matrix
    //If it is, ask whether the user wants to overwrite with a new matrix
    //If yes, return true
    bool overwriteCheck(std::string& key);

    //Get valid matrix input from the user
    //Return false if the user has invalid input, and they choose to quit
    bool getMatrixInput(const std::string& key, std::string& matrixString, size_t& rows, size_t& columns);

    //Get either a 'y' for "yes" or 'n' for "no"
    //If yes, return true
    bool getYesNo() const;
};

#endif //INTERFACE_HPP_
