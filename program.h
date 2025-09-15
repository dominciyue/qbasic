#ifndef PROGRAM_H
#define PROGRAM_H

#include "statement.h"

class program
{
    friend class MainWindow;

public:
    program();
    void insert_stmt(QString _content, int _line_num);
    statement* find(int _line_num);
    int get_num();
    void clear();

private:
    statement* head;
    int stmt_num;
    bool ifadd_new;
};

#endif
