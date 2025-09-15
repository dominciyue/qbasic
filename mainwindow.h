#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include "evalstate.h"
#include "parser.h"
#include "program.h"
#include "tokenizer.h"
#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    EvaluationContext context;

    void run_code(EvaluationContext &context, statement* current_stmt);
    bool judge_if(QString str);
    // 获得要输出表达树的后面部分
    QString syntax_behind(QString &out_tree, Expression* node, int num, EvaluationContext &context);
    // 将枚举移到类定义开始处
    enum CommandType {
        REM,
        LET,
        PRINT,
        INPUT,
        GOTO,
        IF,
        END,
        INVALID
    };

    void initCommandHandlers();
    CommandType parseCommandType(const QString& cmd);
    bool handleLetCommand(statement* stmt);
    bool handlePrintCommand(statement* stmt);
    bool handleIfCommand(statement* stmt);
    bool handleGotoCommand(statement* stmt);
    bool handleInputCommand(statement* stmt);
    bool handleRemCommand(statement* stmt);
private slots:
    void on_cmdLineEdit_editingFinished();
    void clearAll();
    void codeLineEdit_return(QString str = "");
    void load_code();
    void run();
    void setUIForDebugMode();
    void setUIExitDebugMode();
    void debugResume();
    void handleError(statement* stmt, const QString& error);
private:
    Ui::MainWindow *ui;

    parser* Parser;
    program* Program;
    statement* input_stmt;
    CodeTokenizer* Tokenizer;
    // 用来生成表达树时使用
    CodeTokenizer* Tokenizer_tree;

    // 输入变量名
    QString input_name;
    // 输入位置
    statement* input_positi;
    // 是否直接输入
    bool if_in_dir = 0;
    QMap<QString, int> iden_ocurr_num;

    QSet<int> breakpoints;  // 存储断点行号
    bool isDebugMode = false;
    statement* currentDebugStmt = nullptr;
    void updateMonitorDisplay();  // 更新变量监视器
    void executeDebugCode(statement* stmt);  // Debug模式下执行代码
    using CommandHandler = std::function<bool(statement*)>;
    QMap<CommandType, CommandHandler> commandHandlers;
};
#endif // MAINWINDOW_H
