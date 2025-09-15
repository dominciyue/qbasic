#include "program.h"

program::program()
{
    head = new statement;
    stmt_num = 0;
}

void program::clear(){
    statement* x = head;
    statement* y = head->next;
    while(y != nullptr){
        x = y;
        delete y;
        y = x;
        y = y->next;
    }
    stmt_num = 0;
}

statement* program::find(int _line_num){
    statement* root = head;

    if(head->next == nullptr){
        ifadd_new = 1;
        return head;
    }
    else{
        statement* tmp = head->next;
        while(tmp->line_num < _line_num && tmp->next != nullptr){
            tmp = tmp->next;
            root = root->next;
        }

        if(tmp->line_num == _line_num){
            ifadd_new = 0;
            return tmp;
        }
        else if(tmp->line_num > _line_num && tmp->next == nullptr){
            ifadd_new = 1;
            return root;
        }
        else if(tmp->next != nullptr){
            ifadd_new = 1;
            return root;
        }
        else{
            ifadd_new = 1;
            return tmp;
        }
    }
}

void program::insert_stmt(QString _content, int _line_num){

    if(_line_num < 0 || _line_num > 1000000) {
        throw QString("Invalid line number: must be between 1 and 1000000");
    }

    if(_line_num != 0){
        statement* t = find(_line_num);
        if(ifadd_new == 1){
            if(t->next == nullptr){
                stmt_num++;
                t->next = new statement(_content, nullptr);
            }
            else{
                statement* tmp = t;
                t->next = new statement(_content, tmp->next);
                stmt_num++;
            }
        }
        else{
            t->content = _content;
            t->line_num = _content.section(' ', 0, 0).toInt();
        }
    }

    else{
        statement* tmp;
        tmp = head->next;
        while(tmp->next != nullptr) tmp = tmp->next;
        stmt_num++;
        tmp->next = new statement(_content, nullptr);
    }
}

int program::get_num(){
    return stmt_num;
}
