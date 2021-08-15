/*
All operations that involve prompting the user, or taking input from the user are implemented in this file. Here is a
layout of the general interface for Lina.

DEFINE (Optional Arg - Matrix Identifier) : Define a matrix with a user specifed identifier, the identifer can be
established inline with the define command as an optional argument. If not, the user will be asked to enter an
identifier. In the event that the user enters an identifier that is already assigned to a matrix, they will be asked if
they would like to overwrite the existing matrix before defining.

DISPLAY (Optional Args - Matrix Identifiers) : If no arguments are provided, all matrices in the system will be
displayed, otherwise the user can enter as many indentifiers separated by a space as they desire, and all matching
indentifers will display their cooresponding matrices.

CLEAR : Clear the terminal

QUIT : Quit the program, and write all matrices to an external data file

@Sean Siders
sean.siders@icloud.com
*/

#include "Interface.hpp"

Interface::Interface() : recent(nullptr) {}
Interface::Interface(const char* filename) : matrixTree(filename), recent(nullptr) {}

//Prompt the user for input
//Branch to different parts of the program based on the input
//Return false only when user enters 'q' or "quit", to end the program
bool Interface::run()
{
    std::string buffer;

    std::cout << "Lina > ";

    //Read a line from the user
    getline(std::cin, buffer, '\n');

    //Convert to stream to read one word at a time
    std::istringstream stream(buffer);
    std::string initialCommand;

    //First word in the input
    stream >> initialCommand;

    //Determine how the program will respond to the initial command from input
    switch (evaluateCommand(initialCommand))
    {
        case DEFINE : define(stream); break;

        case DISPLAY : display(stream); break;

        case CLEAR : clearScreen(); break;

        case HELP : helpPrompt(); break;

        case QUIT : return false;

        case OPERATE :
        {
            try { operate(initialCommand, stream); }

            catch (const ExceptionHandler& ex)
            {
                std::cout << ex << "\n\n";
            }
            break;
        }

        default : break;
    }

    return true;
}

//Determine which command the user entered, return the respective |Commands|
Commands Interface::evaluateCommand(const std::string& command)
{
    //Variable declaration
    if (("def" == command || "define" == command)) return DEFINE;

    //Display 1 or more matrices
    if (("disp" == command || "display" == command)) return DISPLAY;

    //Clear the console
    if ("clear" == command) return CLEAR;

    //Prompt the user with instructions on using Lina
    if ("help" == command) return HELP;

    //PROGRAM EXIT
    if ("q" == command || "quit" == command) return QUIT;

    //The user may be trying to apply 1 or more operations
    return OPERATE;
}

//Count the number of |columns| and |rows| in |matrixString|
//Check that each entrie contains numeric expressions
//Return false if any rows do not match the 1st row's column count, or an entrie is non-numeric
bool Interface::validMatrixInput(std::string& matrixString, size_t& rows, size_t& columns)
{ 
    //Whether or not the input is valid
    //This will be returned and break loops early if false
    bool isValid = true;

    //|matrixString| iterator
    std::string::const_iterator it = matrixString.begin();

    //The end of |matrixString|
    std::string::const_iterator end = matrixString.end();

    //Count the number of columns in the first row
    while (isValid && it != end && '\n' != *it)
    {
        if (' ' == *it) ++columns;
        else isValid = validChar((*it));
        ++it;
    }

    //Count the first row
    if ('\n' == *it) ++rows;

    //Move to the second row
    if (it != end) ++it;

    //To count the columns of each row
    size_t currentColumns = 1;

    //Count the number of rows to follow
    //Check that each row contains the initial row's column count
    while (isValid && it != end)
    {
        if (' ' == *it) ++currentColumns;

        //Row finished
        else if ('\n' == *it)
        {
            ++rows;
            isValid = (currentColumns == columns);
            currentColumns = 1;
        }

        else isValid = validChar((*it));

        ++it;
    }

    if (*(end - 1) != '\n')
    {
        matrixString += '\n';
        ++rows;
    }

    return isValid;
}

//Return true only if |c| contains a char that is in the pool of valid char input for a matrix entry
//VALID ENTRY CHARS
//Any digit or '-' '.'
bool Interface::validChar(const char c)
{
    return (std::isdigit(c) || '-' == c || '.' == c);
}

//Define a matrix as a particular key, insert it into the database
void Interface::define(std::istringstream& stream)
{
    std::string key;

    //The key was not supplied in the initial command
    if (stream.eof())
    {
        std::cout << "Enter an identifier > ";
        getline(std::cin, key, '\n');
    }
    else stream >> key;

    //Check that |key| is not a command id within the program
    //|OPERATE| is the default return when the input is unique
    while (evaluateCommand(key) != OPERATE)
    {
        std::cout << '\"' << key << "\" is not available to be a matrix id, this is a Lina command id\n"
        << "Lina command ids\n"
        << "clear, def, define, disp, display, help, q, quit\n"
        << "Please choose another id > ";

        getline(std::cin, key, '\n');
    }

    //Check if the key is already defined
    //If it is, ask them if they would like to overwrite
    if (overwriteCheck(key))
    {
        std::string matrixString;
        size_t rows;
        size_t columns;

        if (getMatrixInput(key, matrixString, rows, columns))
        {
            recent = matrixTree.insert(Matrix(key, matrixString, rows, columns));
            
            if (recent) std::cout << "\n\n\"" << key << "\" defined\n\n";
        }
    }
}

//Either display all matrices or specified matrices by key as additional arguments in the |stream|
void Interface::display(std::istringstream& stream) const
{
    //Display all
    if (stream.eof()) std::cout << matrixTree;

    //Extract the specified keys and display any that match a matrix in the |matrixTree|
    else
    {
        std::string key;
        Matrix* retrieved = nullptr;

        while (stream >> key)
        {
            retrieved = matrixTree.retrieve<std::string>(key);
            if (retrieved) std::cout << *retrieved << '\n';
        }
    }
}

//Print 100 newline characters
void Interface::clearScreen() const
{
    for (size_t i = 0; i < 100; ++i) std::cout << '\n';
}

void Interface::helpPrompt() const
{
    //Prompt the user with instructions on how to use Lina
    std::cout << "\nLina\n\n"
    << "==== BASIC COMMANDS ====\n\n"
    << "\"define\" OR \"def\" (*optional arg) -- define a new matrix with a unique *id\n"
    << "\"display\" OR \"disp\" (*optional arg(s)) -- display all matrices or the provided *id(s) separated by a single space\n"
    << "\"clear\" -- clear the terminal\n"
    << "\"help\" (*optional arg) -- display this prompt\n"
    << "\"quit\" OR \"q\" -- terminate the program, saving all defined matrices\n\n"

    << "==== MATRIX OPERATIONS ====\n\n"

    << "ADDITION OR SUBTRACTION\n"
    << "id1 + id2 OR id1 - id2\n"
    << "- all matrices in addition or subtraction must be of the same order\n\n"

    << "Multiplication (Dot Product)\n"
    << "id1 * id2\n"
    << "- The magnitude of columns in the left operand must equal the magnitude of rows in the right operand\n\n";
}

//If the user is attempting an operation |lhsKey| will be:
//  1) A new matrix identifier for the resulting matrix (the operator to follow in |stream| is '=')
//  2) An existing matrix identifier for the resulting matrix, for which overwrite will occur
//  3) The Left operand of the matrix operation
//      * The resulting matrix will just be displayed and not assigned an identifier or stored in the |matrixTree|
void Interface::operate(const std::string& lhsKey, std::istringstream& stream)
{
    //To store the operator read in from |stream|
    std::string op;

    //To store the right operand identifier read in from |stream|
    std::string rhsKey;

    //Attempt to read in an operator from |stream|
    stream >> op;

    //Attempt to read in the right operand from |stream|
    stream >> rhsKey;

    //Attempt |lhs| retrieval on first call
    //For any following call |lhs| will be the |result| of the previous operation
    //|lhs| may or may not be an existing matrix
    Matrix* lhs = matrixTree.retrieve<std::string>(lhsKey);

    //Attempt |rhs| retrieval
    //|rhs| must be an existing matrix
    Matrix* rhs = matrixTree.retrieve<std::string>(rhsKey);
    if (!rhs) throw ExceptionHandler("INVALID COMMAND : enter \"help\" for all valid commands");

    //Evaluate the operater read in, if it is valid, attempt the operation
    switch(evaluateOperator(op))
    {
        case INVALID_OP : throw ExceptionHandler("INVALID OPERATOR : enter \"help\" for all valid commands");

        case PLUS : add(lhs, rhs, true); break;

        case MINUS : add(lhs, rhs, false); break;

        case MULTIPLY : multiply(lhs, rhs); break;

        case ASSIGN : assign(lhs, lhsKey, rhs, stream); break;

        default : return;
    }
}

//Returns the resulting matrix, used in |assign|
void Interface::operate(const Matrix* lhs, const Operators op, const Matrix* rhs, Matrix*& result, const std::string& resultKey)
{
    switch(op)
    {
        case PLUS :
        {
            if (result) result->overwrite((*lhs) + (*rhs), resultKey);
            else result = new Matrix((*lhs) + (*rhs), resultKey);
        }

        case MINUS :
        {
            if (result) result->overwrite((*lhs) + (*rhs), resultKey);
            else result = new Matrix((*lhs) + (*rhs), resultKey);
        }

        case MULTIPLY :
        {
            if (result) result->overwrite((*lhs) + (*rhs), resultKey);
            else result = new Matrix((*lhs) + (*rhs), resultKey);
        }

        default : break;
    }
}

//Evaluate which operator the user input, then return the cooresponding enum
Operators Interface::evaluateOperator(const std::string& operatorString) const
{
    //Attempt matrix addition
    if ("+" == operatorString) return PLUS;

    //Attempt matrix subtraction
    if ("-" == operatorString) return MINUS;

    //Attempt matrix multiplication
    if ("*" == operatorString) return MULTIPLY;

    //Attempt matrix assignment to the following operation
    if ("=" == operatorString) return ASSIGN;
    
    //Invalid command
    return INVALID_OP;
}

//Attempt to add two matrices |lhs| and |rhs|
//If |positive| is false, |rhs| will be subracted from |lhs|
//If the matrices are not of the same order, addition cannot occur, an exception is thrown
void Interface::add(const Matrix* lhs, const Matrix* rhs, const bool positive)
{
    //If |lhs| and |rhs| are not of the same order
    if (!lhs || !lhs->orderMatch(rhs))
        throw InvalidOperation(lhs, (positive ? '+' : '-'), rhs, "Matrices must be of the same order for addition / subtraction");

    std::cout << (positive ? ((*lhs) + (*rhs)) : ((*lhs) - (*rhs)));
}

//Attempt to multiple two matrices |lhs| and |rhs|
//If the columns of |lhs| do not match the rows of |rhs|, multiplication cannot occur
void Interface::multiply(const Matrix* lhs, const Matrix* rhs)
{
    if (!lhs || !lhs->multiplyCheck(rhs))
        throw InvalidOperation(lhs, '*', rhs,
        "The degree of columns in left matrix must match the degree of rows in the right matrix for multiplication");

    std::cout << ((*lhs) * (*rhs));
}

//Attempt to do a matrix operation with |lhs| and the |rhs| which is yet to be read in from |stream|
//If |result| is already allocated, the user will have to decide if they want to overwrite
//If the operation is successful store the resulting matrix in |result|
void Interface::assign(Matrix*& result, const std::string& resultKey, const Matrix* lhs, std::istringstream& stream)
{
    if (!lhs) throw ExceptionHandler("ASSIGNMENT FAILED : the left operand identifier was not found");

    //To store the operator read in from |stream|
    std::string op;

    //To store the identifier for the |rhs| matrix read in from |stream|
    std::string rhsKey;

    stream >> op;
    Operators eOP = evaluateOperator(op);

    //Invalid operator input cases
    if (eOP == INVALID_OP || eOP == ASSIGN) throw ExceptionHandler("ASSIGNMENT FAILED : operator invalid");

    stream >> rhsKey;

    const Matrix* rhs = matrixTree.retrieve<std::string>(rhsKey);
    if (!rhs) throw ExceptionHandler("ASSIGNMENT FAILED : the right operand identifier was not found");

    //If |result| is allocated, ask the user if they want to overwrite
    if (result)
    {
        std::cout << "The identifier \"" << resultKey << '\"' << " is already assigned to a matrix\n"
        << "would you like to overwrite?";

        if (getYesNo())
        {
            operate(lhs, eOP, rhs, result, resultKey);
            std::cout << "\nThe matrix \"" << resultKey << "\" was overwritten\n" << *result;
        }

        else std::cout << "\nThe matrix \"" << resultKey << "\" was not overwritten\n\n";
    }

    else
    {
        operate(lhs, eOP, rhs, result, resultKey);
        std::cout << "NEW MATRIX DEFINED BY CALCULATION\n" << *result;
        matrixTree.insert(*result);
        delete result; //TODO : remove conditional allocation
    }
}

//Check if |key| is already bound to an existing matrix
//If it is, ask whether the user wants to overwrite with a new matrix
//If the |key| is unique, return true, otherwise return false
bool Interface::overwriteCheck(std::string& key)
{
    Matrix* retrieved = matrixTree.retrieve<std::string>(key);

    //If a matrix was retrieved
    if (retrieved)
    {
        //Set the recent pointer
        recent = retrieved;

        std::string matrixString;
        size_t rows;
        size_t columns;

        std::cout << "The identifier \"" << key << "\" is already assigned to a matrix\n"
        << "Would you like to overwrite?";

        if (getYesNo() && getMatrixInput(key, matrixString, rows, columns))
        {
            retrieved->overwrite(Matrix(key, matrixString, rows, columns));
            std::cout << "\n\"" << key << "\" successfully overwritten\n\n";
        }

        return false;
    }

    return true;
}

//Get valid matrix input from the user
//Return false if the user decides to quit
//|matrixString| will always contain a '\n' at the end of the string, '#' is ignored
bool Interface::getMatrixInput(const std::string& key, std::string& matrixString, size_t& rows, size_t& columns)
{
    bool again = false;
    bool validInput = false;

    do
    {
        rows = 0;
        columns = 1;

        std::cout << "DEFINING: \"" << key << "\"\n"
        << "- Enter \"q#\" to quit\n"
        << "- Separate each entry with a single space\n"
        << "- End each row with a single new line\n"
        << "- Finish input with \'#\' then hit enter\n\n"
        << "example input for a 2x3 matrix:\n\n"
        << "11 21 3\n64 12 9\n#\n\n";

        //Get the matrix input
        getline(std::cin, matrixString, '#');
        std::cin.ignore();

        if ('q' == matrixString[0]) return false;

        //Input was invalid, prompt the user
        if (!validMatrixInput(matrixString, rows, columns))
        {
            std::cout << "INVALID MATRIX INPUT\n\nwould you like to try again?";
            again = getYesNo();
            validInput = false;
        }

        //Input was valid, check with the user
        else
        {
            std::cout << "\n\nYOUR MATRIX\n\n" << matrixString << "\n\n"
            << rows << " x " << columns << "\n\nIs this correct?";
            again = !getYesNo();
            validInput = true;
        }

    } while (again);

    return validInput;
}

//Get either a 'y' for "yes" or 'n' for "no"
//If yes, return true
bool Interface::getYesNo() const
{
    std::string buffer;

    std::cout << " (y/n) > "; getline(std::cin, buffer, '\n');
    char response = std::tolower(buffer[0]);

    while ('y' != response && 'n' != response)
    {
        std::cout << "INVALID INPUT : Enter a \'y\' for \"yes\" or \'n\' for \"no\"\n(y/n) > ";
        getline(std::cin, buffer, '\n');
        response = std::tolower(buffer[0]);
    }

    return ('y' == response);
}
