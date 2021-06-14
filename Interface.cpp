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
        case INVALID_CMD : invalidCommand(buffer); break;

        case DEFINE : define(stream); break;

        case DISPLAY : display(stream); break;

        case CLEAR : clearScreen(); break;

        case OPERATE :
        {
            Matrix* result = nullptr;
            if (!operate(stream, result)) invalidCommand(buffer);
            break;
        }

        case QUIT : return false;

        default : break;
    }

    return true;
}

//Determine which command the user entered, return the respective |Commands|
Commands Interface::evaluateCommand(const std::string& command)
{
    // PROGRAM EXIT
    if ("q" == command || "quit" == command) return QUIT;

    //Variable declaration
    if (("d" == command || "def" == command || "define" == command)) return DEFINE;

    //Display 1 or more matrices
    if (("disp" == command || "display" == command)) return DISPLAY;

    //Clear the console
    if (("c" == command || "clear" == command)) return CLEAR;

    //Attempt matrix operation
    if (("op" == command || "operate" == command)) return OPERATE;

    //Invalid command
    return INVALID_CMD;
}

//Prompt the user that their |command| was not valid
void Interface::invalidCommand(const std::string& command)
{
    std::cout << "INVALID COMMAND : \"" << command << "\"\n\n";
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

//1) Retrieve left operand by the identifier
//2) Read in the operator
//3) Retieve Right operand by the identifier
//4) Perform operation
//If a retrieval is unsuccessful, or the operation is invalid, return false
//This algorithm will recursively continue until either invalid input occurs or the end of the stream is reached
//The resulting matrix will be allocated in |result|, where the user can choose to link it to an identifier
bool Interface::operate(std::istringstream& stream, Matrix*& result) const
{
    //To store the operand identifiers read in from |stream|
    std::string identifier;

    //To store the operator read in from |stream|
    std::string op;

    //Attempt to read in left operand identifier
    stream >> identifier;

    if (stream.eof()) return true;

    //Attempt |lhs| retrieval on first call
    //For any following call |lhs| will be the |result| of the previous operation
    Matrix* lhs = (result ? result : matrixTree.retrieve<std::string>(identifier));
    if (!lhs) return false;

    //Attempt to read in the operator
    stream >> op;

    //Attempt to read in right operand identifier
    stream >> identifier;

    //Attempt |rhs| retrieval
    Matrix* rhs = matrixTree.retrieve<std::string>(identifier);
    if (!rhs) return false;

    //TODO exception handling to specify the failed operation

    switch(evaluateOperator(op))
    {
        case INVALID_OP : return false;

        case PLUS :
            if (!add(lhs, rhs, true, result)) return false;
            break;

        case MINUS :
            if (!add(lhs, rhs, false, result)) return false;
            break;

        case MULTIPLY : break;

        default : return false;
    }

    return operate(stream, result);
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
    
    //Invalid command
    return INVALID_OP;
}

//Attempt to add two matrices |lhs| and |rhs|
//If |positive| is false, |rhs| will be subracted from |lhs|
//If the matries are not of the same order, addition cannot occur, return false
bool Interface::add(const Matrix* lhs, const Matrix* rhs, const bool positive, Matrix*& result) const
{
    //If |lhs| and |rhs| are not of the same order
    if (!lhs->orderMatch(rhs)) return false;

    //Matrix addition
    if (positive)
    {
        //For the first operation a new matrix must be allocated
        if (!result) result = new Matrix(*lhs + *rhs);

        else *result += *rhs;

        std::cout << "\n\nRESULT\n\n";
        result->debugDisplay();
    }

    //Matrix subtraction 
    else
    {
        //For the first operation a new matrix must be allocated
        if (!result) result = new Matrix(*lhs - *rhs);

        else *result -= *rhs;

        std::cout << "\n\nRESULT\n\n";
        result->debugDisplay();
    }
    
    delete result;

    return true;
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
bool Interface::getYesNo()
{
    std::string buffer;

    std::cout << " (y/n) > ";
    getline(std::cin, buffer, '\n');
    char response = std::tolower(buffer[0]);

    while ('y' != response && 'n' != response)
    {
        std::cout << "INVALID INPUT : Enter a \'y\' for \"yes\" or \'n\' for \"no\"\n(y/n) > ";
        getline(std::cin, buffer, '\n');
        response = std::tolower(buffer[0]);
    }

    return ('y' == response);
}

