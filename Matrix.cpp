#include "Matrix.hpp"

std::ostream& operator<<(std::ostream& out, const Matrix& rhs)
{
    rhs.display(out);
    return out;
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

//Allocate |matrix| to the dimensions supplied with |rows| and |columns|
//Populate the |matrix| with the numeric entries in |matrixString|
Matrix::Matrix(const std::string& identifier, const std::string& matrixString, const size_t& rows, const size_t& columns) :
    identifier(identifier), matrixString(matrixString), rows(rows), columns(columns), matrix(nullptr)
{
    std::istringstream stream(matrixString);
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

Matrix::~Matrix()
{
    for (size_t r = 0; r < rows; ++r)
    {
        delete [] matrix[r];
        matrix[r] = nullptr;
    }

    
    delete [] matrix;
    matrix = nullptr;
}

void Matrix::display(std::ostream& out) const
{
    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = 0; c < columns; ++c)
            out << matrix[r][c] << " ";

        out << '\n';
    }
}

void Matrix::copy(const Matrix& source)
{
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
