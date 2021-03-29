#include "Interface.hpp"

Interface::Interface() : recent(nullptr) {}

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
        case INVALID : invalidCommand(buffer); break;

        case DEFINE : define(stream); break;

        case DISPLAY : display(stream); break;

        case CLEAR : clearScreen(); break;

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

    if (("disp" == command || "display" == command)) return DISPLAY;

    if (("c" == command || "clear" == command)) return CLEAR;

    return INVALID;
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
//VALID ENTRIE CHARS
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

