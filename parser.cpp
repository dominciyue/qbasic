#include "parser.h"
#include <QSet>
parser::parser()
{
}

bool parser::isNumber(QString str){
    bool ok;
    str.toInt(&ok);
    return ok;
}

bool parser::isOperator(QString str){
    if(str == "=" || str == "+" || str == "-" || str == "*" || str == "/" || str == "(" || str == ")" || str == "**" || str == "MOD"){
        return true;
    }
    return false;
}


bool parser::isVariable(QString str) {
    return isValidVariableName(str);
}

int parser::getOperatorPriority(QString _operator){
    if(_operator == "=") return 1;
    if(_operator == "+" || _operator == "-") return 2;
    if(_operator == "*" || _operator == "/" || _operator == "MOD") return 3;
    if(_operator == "**") return 4;
    // 对应操作符为左括号情况
    return 0;
}

Expression* parser::parseExpression(QList<QString> tokens){
    if(tokens[0] == "-") tokens.insert(0, "0");
    for(int i = 0; i < tokens.length(); i++){
        if(tokens[i] == "(" && tokens[i + 1] == "-"){
            tokens.insert(i + 1, "0");
        }
    }

    Expression* tree = nullptr;
    for(int i = 0; i < tokens.length(); i++){
        QString t = tokens[i];
        if(isNumber(t)){
            int val = t.toInt();
            NumericConstant* const_exp = new NumericConstant(val);
            Operands.push(const_exp);
            continue;
        }
        // 变量的进栈处理
        if(isVariable(t)){
            VariableExp* ident_exp = new VariableExp(t);
            Operands.push(ident_exp);
            continue;
        }
        if(isOperator(t)){
            // 左括号直接进栈
            if(t == "("){
                Operators.push(t);
                continue;
            }
            // 右括号以外的情况
            else if(t != ")"){
                // 负号在表达式头部
                if(t == "-" && i == 0){
                    if(isNumber(tokens[1]) || isVariable(tokens[1])){
                       if(isVariable(tokens[1])){
                           VariableExp* ident_exp = new VariableExp(tokens[1]);
                           Operands.push(ident_exp);
                       }
                       if(isNumber(tokens[1])){
                           NumericConstant* const_exp = new NumericConstant(tokens[1].toInt() * (-1));
                           Operands.push(const_exp);
                       }
                       i = 1;
                    }
                    // else error;
                }

                if(Operators.empty()) Operators.push(t);
                // 操作符栈不为空时
                else{
                    int new_prior = getOperatorPriority(t);
                    int top_prior = getOperatorPriority(Operators.top());
                    // 直接入栈
                    if((new_prior > top_prior) || (new_prior == top_prior && new_prior == 4)) Operators.push(t);

                    else{
                        // 处理后入栈
                        do{
                            Expression* left;
                            Expression* right;

                            if(!Operands.empty()){
                                right = Operands.top();
                                Operands.pop();
                                if(!Operands.empty()){
                                    left = Operands.top();
                                    Operands.pop();
                                }
                                else left = nullptr;
                            }
                            else{
                                left = nullptr;
                                right = nullptr;
                            }

                            QString top_opera = Operators.top();
                            Operators.pop();
                            // 生成compound expression
                            OperatorExp* compo_exp = new OperatorExp(top_opera, left, right);
                            Operands.push(compo_exp);

                            if(!Operators.empty()) top_prior = getOperatorPriority(Operators.top());
                            else break;
                        }while(new_prior < top_prior);
                        Operators.push(t);
                    }
                }
            }
            else if(t == ")"){
                while(Operators.top() != "("){
                    Expression* left;
                    Expression* right;

                    if(!Operands.empty()){
                        right = Operands.top();
                        Operands.pop();
                        if(!Operands.empty()){
                            left = Operands.top();
                            Operands.pop();
                        }
                        else left = nullptr;
                    }
                    else{
                        left = nullptr;
                        right = nullptr;
                    }

                    QString top_opera = Operators.top();
                    Operators.pop();
                    OperatorExp* compo_exp = new OperatorExp(top_opera, left, right);
                    Operands.push(compo_exp);
                }
                Operators.pop();
            }
        }
    }

    while(!Operators.empty()){
        Expression* left;
        Expression* right;

        if(!Operands.empty()){
            right = Operands.top();
            Operands.pop();
            if(!Operands.empty()){
                left = Operands.top();
                Operands.pop();
            }
            else left = nullptr;
        }
        else{
            left = nullptr;
            right = nullptr;
        }

        QString top_opera = Operators.top();
        Operators.pop();

        OperatorExp* compo_exp = new OperatorExp(top_opera, left, right);
        Operands.push(compo_exp);
    }
    tree = Operands.top();
    return tree;
}
bool parser::isKeyword(const QString& str) {
    static const QSet<QString> keywords = {
        "REM", "LET", "PRINT", "INPUT", "GOTO", "IF", "THEN", "END", "MOD"
    };
    return keywords.contains(str.toUpper());
}

bool parser::isValidVariableName(const QString& str) {
    if(str.isEmpty() || isKeyword(str)) return false;

    QChar first = str[0];
    if(!first.isLetter()) return false;

    for(int i = 1; i < str.length(); i++) {
        if(!str[i].isLetterOrNumber()) return false;
    }
    return true;
}
