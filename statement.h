#ifndef STATEMENT_H
#define STATEMENT_H

#include <QString>

class statement
{
    friend class MainWindow;
    friend class program;

public:
    statement();
    statement(QString input, statement* _next = nullptr);
    QString get_content();
    int get_linenum();
    void repeated_line_cover(QString _input);

private:
    QString content;
    // 输入语句的行号
    QString pre;
    int line_num = 0;
    statement* next;
    QString syntax_tree;
    int if_first;
    int trigger_num;
    // 记录if语句的触发
    int if_tri_true;
    int if_tri_false;
    bool if_error = 0;
};

#endif // STATEMENT_H
