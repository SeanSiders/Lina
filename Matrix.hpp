/*
@Sean Siders
sean.siders@icloud.com
*/

#ifndef MATRIX_HPP_ 
#define MATRIX_HPP_

#include <iostream>
#include <sstream>
#include <fstream>

//// FORWARD DECLARATION
class Matrix;

//// GLOBAL OPERATOR OVERLOADS
std::ostream& operator<<(std::ostream& out, const Matrix& rhs);
std::ofstream& operator<<(std::ofstream& outFile, const Matrix& rhs);

class Matrix
{
    public:
    //// DEBUG 
    void debugDisplay() const;

    friend std::ostream& operator<<(std::ostream& out, const Matrix& rhs);
    friend std::ofstream& operator<<(std::ofstream& outFile, const Matrix& rhs);
    bool operator<(const Matrix& rhs) const;
    bool operator>(const Matrix& rhs) const;
    bool operator<(const std::string& rhs) const;
    bool operator>(const std::string& rhs) const;
    Matrix& operator=(const Matrix& rhs);

    //// MATHEMATIC MATRIX OPERATORS

    //Addition
    Matrix operator+(const Matrix& rhs) const;
    Matrix& operator+=(const Matrix& rhs);

    //Subtraction
    Matrix operator-(const Matrix& rhs) const;
    Matrix& operator-=(const Matrix& rhs);

    //Multiplication
    Matrix operator*(const Matrix& rhs) const;
    Matrix& operator*=(const Matrix& rhs);

    Matrix();
    Matrix(const Matrix& source);
    Matrix(const Matrix& source, const std::string& identifier);
    Matrix(std::ifstream& inFile);
    ~Matrix();

    //Allocate |matrix| to the dimensions supplied with |rows| and |columns|
    //Populate the |matrix| with the numeric entries in |matrixString|
    Matrix(const std::string& identifier, const std::string& matrixString, const size_t& rows, const size_t& columns);

    //Display the matrix |identifier| followed by the |matrixString|
    void display(std::ostream& out = std::cout) const;

    //Display only the matrix |identifier|
    void displayIdentifier(std::ostream& out = std::cout) const;

    //Clear the current matrix and make a copy of |source| into this matrix
    void overwrite(const Matrix& source);
    void overwrite(const Matrix& source, const std::string& newIdentifier);

    //Write the contents of of this matrix out to |outFile|
    void writeFile(std::ofstream& outFile) const;

    //True if the order of |other| matches to order of this matrix
    bool orderMatch(const Matrix* other) const;

    //For checking whether this matrix can multiply with |other|
    //True if the degree of columns of this matrix match the degree of rows of |other|
    bool multiplyCheck(const Matrix* other) const;

    //Set all members to initial values
    //Deallocate the |matrix|
    void clear();

    //Deallocate the |matrix| and set to null
    void clearMatrix();

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

    //Allocate the |matrix| to the dimensions of |rows| x |columns|
    //Read in entries from the provided |stream|
    void readIn(std::istream& stream);

    //Make a copy of |source| into this matrix
    void copy(const Matrix& source);

    //Copy the |matrix| from |source|
    void copyMatrix(const Matrix& source);

    //Multiply the current |matrix| with |other| as a |product| matrix
    //Simultaneously deallocate |matrix| and modify |matrixString|
    //Return the |product| matrix
    double** multiply(const Matrix& rhs);
};

#endif //MATRIX_HPP_
