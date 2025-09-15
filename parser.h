#ifndef PARSER_H
#define PARSER_H

#include "exp.h"
#include <stack>
#include <QList>

class parser
{
public:
    parser();
    Expression* parseExpression(QList<QString> tokens);
    bool isNumber(QString str);
    bool isOperator(QString str);
    bool isVariable(QString str);
    int getOperatorPriority(QString op);

private:
    // 储存操作数和操作符
    stack<Expression*> Operands;
    stack<QString> Operators;
    bool isKeyword(const QString& str);
    bool isValidVariableName(const QString& str);
};

#endif
