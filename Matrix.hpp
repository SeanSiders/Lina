#ifndef MATRIX_HPP_ 
#define MATRIX_HPP_

#include <iostream>
#include <sstream>

class Matrix;

std::ostream& operator<<(std::ostream& out, const Matrix& rhs);

class Matrix
{
    public:
    friend std::ostream& operator<<(std::ostream& out, const Matrix& rhs);
    Matrix& operator=(const Matrix& rhs);

    Matrix();
    Matrix(const Matrix& source);
    ~Matrix();

    //Allocate |matrix| to the dimensions supplied with |rows| and |columns|
    //Populate the |matrix| with the numeric entries in |matrixString|
    Matrix(const std::string& identifier, const std::string& matrixString, const size_t& rows, const size_t& columns);

    private:
    //The user-defined identifier that is related to this matrix
    //This is also the key value that matrices are sorted in |Tree.hpp|
    std::string identifier;

    //The string version of the matrix, for printing purposes
    std::string matrixString;

    //The number of rows in the matrix
    size_t rows;

    //The number of columns in the matrix
    size_t columns;

    //The matrix that contains all numeric values
    double**  matrix;

    void display(std::ostream& out) const;
    void copy(const Matrix& source);
};

#endif //MATRIX_HPP_
