#include "Matrix.hpp"

void Matrix::debugDisplay() const
{
    std::cout << '\n';

    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = 0; c < columns; ++c)
            std::cout << matrix[r][c] << ' ';

        std::cout << '\n';
    }

    std::cout << '\n';
}

std::ostream& operator<<(std::ostream& out, const Matrix& rhs)
{
    rhs.display(out);
    return out;
}

std::ofstream& operator<<(std::ofstream& outFile, const Matrix& rhs)
{
    rhs.writeFile(outFile);
    return outFile;
}

bool Matrix::operator<(const Matrix& rhs) const
{
    return identifier < rhs.identifier;
}

bool Matrix::operator>(const Matrix& rhs) const
{
    return identifier > rhs.identifier;
}

bool Matrix::operator<(const std::string& rhs) const
{
    return identifier < rhs;
}

bool Matrix::operator>(const std::string& rhs) const
{
    return identifier > rhs;
}

Matrix& Matrix::operator=(const Matrix& rhs)
{
    if (this != &rhs) copy(rhs);

    return *this;
}

Matrix Matrix::operator+(const Matrix& rhs) const
{
    //Make a copy of this matrix
    Matrix result(*this);

    //Add |rhs|
    result += rhs;

    return result;
}

Matrix& Matrix::operator+=(const Matrix& rhs)
{
    //Reset the matrix string
    matrixString.clear();

    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = 0; c < columns; ++c)
        {
            matrix[r][c] += rhs.matrix[r][c];

            //Append entry to the string
            matrixString += std::to_string(matrix[r][c]);

            //If this is the last column, add a newline to the string
            //Otherwise add a space
            matrixString += (1 + c == columns) ? '\n' : ' ';
        }
    }

    return *this;
}

Matrix Matrix::operator-(const Matrix& rhs) const
{
    //Make a copy of this matrix
    Matrix result(*this);

    //Subtract |rhs|
    result -= rhs;

    return result;
}

Matrix& Matrix::operator-=(const Matrix& rhs)
{
    //Reset the matrix string
    matrixString.clear();

    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = 0; c < columns; ++c)
        {
            matrix[r][c] -= rhs.matrix[r][c];

            //Append entry to the string
            matrixString += std::to_string(matrix[r][c]);

            //If this is the last column, add a newline to the string
            //Otherwise add a space
            matrixString += (1 + c == columns) ? '\n' : ' ';
        }
    }

    return *this;
}

Matrix Matrix::operator*(const Matrix& rhs) const
{
    //Make a copy of this matrix
    Matrix result(*this);

    //Multiply with |rhs|
    result *= rhs;

    return result;
}

Matrix& Matrix::operator*=(const Matrix& rhs)
{
    //Reset the matrix string
    matrixString.clear();

    //Get product matrix and reassign |matrix|
    matrix = multiply(rhs);

    return *this;
}

//Multiply the current |matrix| with |other| as a |product| matrix
//Simultaneously deallocate |matrix| and modify |matrixString|
//Return the |product| matrix
double** Matrix::multiply(const Matrix& rhs)
{
    //Make a new matrix with the new order
    double** product = new double*[rows];

    //Get the column magnitude for the product matrix
    size_t newColumns = rhs.columns;

    //To hold the product sum of each traversal
    double productSum = 0.0;

    //Traverse the rows of this matrix
    for (size_t r = 0; r < rows; ++r)
    {
        //Allocate product matrix row
        product[r] = new double[newColumns];

        //Traverse the columns of |rhs|
        for (size_t i = 0; i < newColumns; ++i)
        {
            productSum = 0.0;

            //Traverse the columns of this matrix and rows of |rhs|
            for (size_t j = 0; j < columns; ++j)
            {
                //Sum the product of columns of this matrix and rows of |rhs|
                productSum += matrix[r][j] * rhs.matrix[j][i];
            }

            product[r][i] = productSum;

            //Append entry to the string
            matrixString += std::to_string(product[r][i]);

            //If this is the last column, add a newline to the string
            //Otherwise add a space
            matrixString += (1 + i == newColumns) ? '\n' : ' ';
        }

        //Dellocate row
        delete [] matrix[r];
        matrix[r] = nullptr;
    }

    //Set the new columns
    columns = newColumns;

    //Deallocate the old matrix
    delete [] matrix;
    matrix = nullptr;

    return product;
}

Matrix::Matrix() : rows(0), columns(0), matrix(nullptr) {}

Matrix::Matrix(const Matrix& source)
{
    copy(source);
}

Matrix::Matrix(const Matrix& source, const std::string& identifier) : identifier(identifier)
{
    matrixString = source.matrixString;
    rows = source.rows;
    columns = source.columns;

    copyMatrix(source);
}

Matrix::Matrix(std::ifstream& inFile)
{
    //Read in the data
    inFile >> identifier;
    inFile >> rows;
    inFile >> columns;

    //Ignore the newline after the first 3 entries
    inFile.ignore(1, '\n');

    //Read in the matrix
    getline(inFile, matrixString, '#');

    std::istringstream stream(matrixString);
    readIn(stream);
}

//Allocate |matrix| to the dimensions supplied with |rows| and |columns|
//Populate the |matrix| with the numeric entries in |matrixString|
Matrix::Matrix(const std::string& identifier, const std::string& matrixString, const size_t& rows, const size_t& columns) :
    identifier(identifier), matrixString(matrixString), rows(rows), columns(columns), matrix(nullptr)
{
    std::istringstream stream(matrixString);
    readIn(stream);
}

Matrix::~Matrix()
{
    clear();
}

//Display the matrix |identifier| followed by the |matrixString|
void Matrix::display(std::ostream& out) const
{
    out << '\"' << identifier << "\"\n" << matrixString;
}

//Display only the matrix |identifier|
void Matrix::displayIdentifier(std::ostream& out) const
{
    out << '\"' << identifier << '\"';
}

//Clear the current matrix and make a copy of |source| into this matrix
void Matrix::overwrite(const Matrix& source)
{
    clear();
    copy(source);
}

void Matrix::overwrite(const Matrix& source, const std::string& newIdentifier)
{
    clear();

    identifier = newIdentifier;
    matrixString = source.matrixString;
    rows = source.rows;
    columns = source.columns;

    copyMatrix(source);
}

//Allocate the |matrix| to the dimensions of |rows| x |columns|
//Read in entries from the provided |stream|
void Matrix::readIn(std::istream& stream)
{
    double entry = 0.0;
    matrix = new double*[rows];
    for (size_t r = 0; r < rows; ++r)
    {
        matrix[r] = new double[columns];
        for (size_t c = 0; c < columns; ++c)
        {
            stream >> entry;
            matrix[r][c] = entry;
        }
    }
}

//Write the contents of of this matrix out to |outFile|
void Matrix::writeFile(std::ofstream& outFile) const
{
    outFile << identifier <<  ' ' << rows << ' ' << columns << '\n' << matrixString << '#';
}

//Make a copy of |source| into this matrix
void Matrix::copy(const Matrix& source)
{
    identifier = source.identifier;
    matrixString = source.matrixString;
    rows = source.rows;
    columns = source.columns;

    copyMatrix(source);
}

//Copy the |matrix| from |source|
void Matrix::copyMatrix(const Matrix& source)
{
    matrix = new double*[rows];
    for (size_t r = 0; r < rows; ++r)
    {
        matrix[r] = new double[columns];

        for (size_t c = 0; c < columns; ++c)
            matrix[r][c] = source.matrix[r][c];
    }
}

//Set all members to initial values
//Deallocate the |matrix|
void Matrix::clear()
{
    identifier.clear();
    matrixString.clear();
    clearMatrix();
}

//Deallocate the |matrix| and set to null
void Matrix::clearMatrix()
{
    for (size_t r = 0; r < rows; ++r)
    {
        delete [] matrix[r];
        matrix[r] = nullptr;
    }

    rows = 0;
    columns = 0;
    
    delete [] matrix;
    matrix = nullptr;
}

//True if the order of |other| matches to order of this matrix
//Also false if |other| is null
bool Matrix::orderMatch(const Matrix* other) const
{
    return (other && other->rows == rows && other->columns == columns);
}

//For checking whether this matrix can multiply with |other|
//True if the degree of columns of this matrix match the degree of rows of |other|
bool Matrix::multiplyCheck(const Matrix* other) const
{
    return (other && columns == other->rows);
}
