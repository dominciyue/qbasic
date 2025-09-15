#ifndef EXP_H
#define EXP_H

#include <string>
#include <QString>
#include <map>
#include "tokenizer.h"
#include "evalstate.h"
#include <QtMath>

class EvaluationContext;

enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND };

class Expression {
public:
   Expression();
   virtual ~Expression();
   virtual int eval(EvaluationContext &context) = 0;
   virtual ExpressionType type() = 0;
   virtual QString getIdentifierName() = 0;
   virtual QString getOperator() = 0;
   virtual Expression *getLHS() = 0;
   virtual Expression *getRHS() = 0;
};

class NumericConstant: public Expression {
public:
   NumericConstant(int val);

   int eval(EvaluationContext & context);
   ExpressionType type();
   virtual QString getIdentifierName();

   QString getOperator();
   Expression *getLHS();
   Expression *getRHS();

private:
   int numValue;

};

class VariableExp: public Expression {
public:
   VariableExp(QString name);

   int eval(EvaluationContext & context);
   ExpressionType type();

   QString getIdentifierName();

   QString getOperator();
   Expression *getLHS();
   Expression *getRHS();

private:

    QString varName;
    int useCount;
};

class OperatorExp: public Expression {
public:
   OperatorExp(QString op, Expression *lhs, Expression *rhs);
   ~OperatorExp();

   int eval(EvaluationContext & context);

   ExpressionType type();

   QString getOperator();
   Expression *getLHS();
   Expression *getRHS();
   virtual QString getIdentifierName();


private:
QString opSymbol;
Expression *lhs, *rhs;

};


#endif // EXP_H
