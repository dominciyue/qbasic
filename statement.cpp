#include "statement.h"
#include <QString>
#include <QStringList>
statement::statement()
{
    next = nullptr;
    syntax_tree.clear();
    if_first = true;
    trigger_num = 0;
    if_tri_true = 0;
    if_tri_false = 0;
    if_error = false;
    line_num = -1;
    content.clear();   // 显式初始化
    pre.clear();      // 显式初始化
}

statement::statement(QString input, statement* _next)
{
    if_first = true;
    trigger_num = 0;
    if_tri_true = 0;
    if_tri_false = 0;
    if_error = false;

    next = _next;
    content = input;
    syntax_tree.clear();

    // 修改行号解析方式
    pre = input.section(' ', 0, 0);
    bool ok;
    int num = pre.toInt(&ok);
    line_num = ok ? num : 0;
}

void statement::repeated_line_cover(QString _input){
    content = _input;
    line_num = _input.section(' ', 0, 0).toInt();
    pre = _input.section(' ', 0, 0);
    syntax_tree = "";
    if_first = 1;
    trigger_num = 0;
    if_tri_true = 0;
    if_tri_false = 0;
}

QString statement::get_content(){
    QString result = content;
    return result.trimmed();
}

int statement::get_linenum(){
    bool ok;
    int num = content.section(' ', 0, 0).toInt(&ok);
    return ok ? num : 0;
}
