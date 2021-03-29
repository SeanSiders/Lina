#include "Matrix.hpp"

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

Matrix::Matrix() : rows(0), columns(0), matrix(nullptr) {}

Matrix::Matrix(const Matrix& source)
{
    copy(source);
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

//Clear the current matrix and make a copy of |source| into this matrix
void Matrix::overwrite(const Matrix& source)
{
    clear();
    copy(source);
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
