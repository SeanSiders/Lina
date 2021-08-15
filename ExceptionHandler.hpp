/*
@Sean Siders
sean.siders@icloud.com
*/

#ifndef EXCEPTION_HANDLER_HPP_
#define EXCEPTION_HANDLER_HPP_

#include <iostream>
#include "Matrix.hpp"

//// FORWARD DECLARATIONS
class ExceptionHandler;
class InvalidOperation;

//// GLOBAL OVERLOAD 

std::ostream& operator<<(std::ostream&, const ExceptionHandler&);
std::ostream& operator<<(std::ostream&, const InvalidOperation&);

class ExceptionHandler
{
    public:
    //////// OPERATOR OVERLOAD 

    friend std::ostream& operator<<(std::ostream& out, const ExceptionHandler& ex);

    ExceptionHandler();
    explicit ExceptionHandler(const std::string& message);

    protected:
    std::string message;
};

class InvalidOperation : public ExceptionHandler
{
    public:
    //////// OPERATOR OVERLOAD 

    friend std::ostream& operator<<(std::ostream& out, const InvalidOperation& ex);

    //////// CONSTRUCTORS 
    InvalidOperation();
    InvalidOperation(const Matrix* lhs, const char op, const Matrix* rhs, const std::string& message);

    private:
    const Matrix* lhs;
    const char op;
    const Matrix* rhs;
};

#endif //EXCEPTION_HANDLER_HPP_
