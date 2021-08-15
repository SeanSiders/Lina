/*
@Sean Siders
sean.siders@icloud.com
*/

#include "ExceptionHandler.hpp"

//Display |message|
std::ostream& operator<<(std::ostream& out, const ExceptionHandler& ex)
{
    out << ex.message;
    return out;
}

std::ostream& operator<<(std::ostream& out, const InvalidOperation& ex)
{
    out << "\nINVALID OPERATION : ";
    ex.lhs->displayIdentifier(out);
    out << " " << ex.op << " ";
    ex.rhs->displayIdentifier(out);
    out << '\n';
    out << ex.message;

    return out;
}


ExceptionHandler::ExceptionHandler() {}

//Initialize |message|
ExceptionHandler::ExceptionHandler(const std::string& message) : message(message)
{
}

InvalidOperation::InvalidOperation() : lhs(nullptr), op('\0'), rhs(nullptr) {}

InvalidOperation::InvalidOperation(const Matrix* lhs, const char op, const Matrix* rhs, const std::string& message) :
    ExceptionHandler(message), lhs(lhs), op(op), rhs(rhs)
{
}
