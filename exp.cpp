#include "exp.h"
Expression::Expression()
{
}

Expression::~Expression()
{
}

NumericConstant::NumericConstant(int val)
{
    numValue = val;
}

ExpressionType NumericConstant::type()
{
    return CONSTANT;
}

QString NumericConstant::getIdentifierName(){
    return " ";
}

QString NumericConstant::getOperator(){
    return " ";
}

Expression* NumericConstant::getLHS(){
    Expression* exp = nullptr;
    return exp;
}

Expression* NumericConstant::getRHS(){
    Expression* exp = nullptr;
    return exp;
}

VariableExp::VariableExp(QString name)
{
    this->varName = name;
    this->useCount = 0;
}

ExpressionType VariableExp::type()
{
    return IDENTIFIER;
}

QString VariableExp::getIdentifierName()
{
    return this->varName;
}

QString VariableExp::getOperator(){
    return " ";
}

Expression* VariableExp::getLHS(){
    Expression* exp = nullptr;
    return exp;
}

Expression* VariableExp::getRHS(){
    Expression* exp = nullptr;
    return exp;
}

OperatorExp::OperatorExp(QString op, Expression *lhs, Expression *rhs)
{
    this->opSymbol = op;
    this->lhs = lhs;
    this->rhs = rhs;
}

OperatorExp::~OperatorExp()
{
}

ExpressionType OperatorExp::type()
{
    return COMPOUND;
}

QString OperatorExp::getOperator()
{
    return opSymbol;
}

Expression* OperatorExp::getLHS()
{
    return lhs;
}

Expression* OperatorExp::getRHS()
{
    return rhs;
}

QString OperatorExp::getIdentifierName()
{
    return " ";
}

int NumericConstant::eval(EvaluationContext & context)
{
   return numValue;
}

int VariableExp::eval(EvaluationContext & context)
{
   if(!context.isDefined(varName)){
       QString error;
       error += varName;
       error += " is not defined";
       throw error;
   }
   else return context.getValue(varName);
}

int OperatorExp::eval(EvaluationContext & context)
{
   if (rhs == nullptr || lhs == nullptr){
       QString error = "Illegal operator which doesn't have a right value";
       throw error;
   }
   int right = rhs->eval(context);
   if (opSymbol == "=") {
      context.setValue(lhs->getIdentifierName(), right);
      return right;
   }
   int left = lhs->eval(context);
   if (opSymbol == "+") return left + right;
   if (opSymbol == "-") return left - right;
   if (opSymbol == "*") return left * right;
   if (opSymbol == "/") {
      if (right == 0){
          QString error = "devide 0";
          throw error;
      }
      return left / right;
   }
   if (opSymbol == "MOD"){
       return left % right;
   }
   if (opSymbol == "**"){
       if (right < 0 && left == 0){
           QString error = "wrong";
           throw error;
       }
       else if (right == 0 && left == 0){
           QString error = "wrong";
           throw error;
       }
       else return qPow(left, right);
   }
   QString error = "Illegal operator";
   throw error;
}
