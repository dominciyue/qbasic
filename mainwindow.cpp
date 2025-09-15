#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUIExitDebugMode();
    Parser = new parser;
    Program = new program;
    Tokenizer = new CodeTokenizer;
    Tokenizer_tree = new CodeTokenizer;
    initCommandHandlers();
    input_stmt = Program->head;

    connect(ui->btnClearCode, SIGNAL(clicked()), this, SLOT(clearAll()));
    connect(ui->cmdLineEdit, SIGNAL(returnPressed()), this, SLOT(codeLineEdit_return()));
    connect(ui->btnLoadCode, SIGNAL(clicked()), this, SLOT(load_code()));
    connect(ui->btnRunCode, SIGNAL(clicked()), this, SLOT(run()));
    connect(ui->btnDebugMode, &QPushButton::clicked, this, &MainWindow::setUIForDebugMode);
    connect(ui->btnExitDebugMode, &QPushButton::clicked, this, &MainWindow::setUIExitDebugMode);
    connect(ui->btnDebugResume, &QPushButton::clicked, this, &MainWindow::debugResume);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cmdLineEdit_editingFinished()
{
}
MainWindow::CommandType MainWindow::parseCommandType(const QString& cmd) {
    QString command = cmd.section(" ", 1, 1);
    if(command == "LET") return CommandType::LET;
    if(command == "PRINT") return CommandType::PRINT;
    if(command == "IF") return CommandType::IF;
    if(command == "GOTO") return CommandType::GOTO;
    if(command == "INPUT") return CommandType::INPUT;
    if(command == "REM") return CommandType::REM;
    if(command == "END") return CommandType::END;
    return CommandType::INVALID;
}

void MainWindow::initCommandHandlers() {
    commandHandlers.insert(CommandType::LET, [this](statement* stmt) {
        return handleLetCommand(stmt);
    });

    commandHandlers.insert(CommandType::PRINT, [this](statement* stmt) {
        return handlePrintCommand(stmt);
    });

    commandHandlers.insert(CommandType::IF, [this](statement* stmt) {
        return handleIfCommand(stmt);
    });

    commandHandlers.insert(CommandType::GOTO, [this](statement* stmt) {
        return handleGotoCommand(stmt);
    });

    commandHandlers.insert(CommandType::INPUT, [this](statement* stmt) {
        return handleInputCommand(stmt);
    });

    commandHandlers.insert(CommandType::REM, [this](statement* stmt) {
        return handleRemCommand(stmt);
    });
}
void MainWindow::handleError(statement* stmt, const QString& error) {
    stmt->if_error = 1;
    QString errorMsg = QString::number(stmt->line_num) + ", " + error;
    ui->textBrowser->append(errorMsg);
}
bool MainWindow::handleLetCommand(statement* stmt) {
    QString con = stmt->content.section(" ", 2, -1);
    QList<QString> _tokens;
    _tokens.clear();
    _tokens = Tokenizer->tokenize(con);
    Expression* _tree;
    _tree = Parser->parseExpression(_tokens);
    try {
        _tree->eval(context);
        stmt->trigger_num++;
        return true;  // 执行成功，继续执行下一条语句
    } catch (QString error) {
        handleError(stmt, error);
        return false;  // 执行失败，停止执行
    }
}

bool MainWindow::handlePrintCommand(statement* stmt) {
    QString con = stmt->content.section(" ", 2, -1);
    QList<QString> _tokens;
    _tokens.clear();
    _tokens = Tokenizer->tokenize(con);
    Expression* _tree;
    _tree = Parser->parseExpression(_tokens);
    try {
        int result = _tree->eval(context);
        ui->textBrowser->append(QString::number(result));
        stmt->trigger_num++;
        return true;
    } catch (QString error) {
        handleError(stmt, error);
        return false;
    }
}

bool MainWindow::handleIfCommand(statement* stmt) {
    QString if_stat = stmt->content.section(" ", 2, -3);
    bool flag = judge_if(if_stat);
    if(flag) {
        stmt->if_tri_true++;
        QString then_line_num = stmt->content.section(" ", -1, -1);
        int des_line = then_line_num.toInt();
        statement* target = Program->find(des_line);
        if(target) {
            currentDebugStmt = target;
            return false;  // 需要跳转，不继续执行下一条语句
        }
    } else {
        stmt->if_tri_false++;
        currentDebugStmt = stmt->next;
    }
    return true;
}

bool MainWindow::handleGotoCommand(statement* stmt) {
    int line = stmt->content.section(" ", 2, 2).toInt();
    statement* target = Program->find(line);
    if(target) {
        currentDebugStmt = target;
        stmt->trigger_num++;
        return false;  // 需要跳转，不继续执行下一条语句
    }
    return true;
}

bool MainWindow::handleInputCommand(statement* stmt) {
    QString varName = stmt->content.section(" ", 2, -1);
    ui->cmdLineEdit->setText("?");
    input_name = varName;
    input_positi = stmt;
    if_in_dir = true;
    stmt->trigger_num++;
    return false;  // 需要等待输入，不继续执行下一条语句
}

bool MainWindow::handleRemCommand(statement* stmt) {
    stmt->trigger_num++;
    return true;  // REM 命令直接继续执行下一条语句
}

void MainWindow::setUIForDebugMode(){
    isDebugMode = true;
    ui->btnClearCode->setVisible(false);
    ui->btnLoadCode->setVisible(false);
    ui->btnDebugMode->setVisible(false);

    ui->btnExitDebugMode->setVisible(true);
    ui->btnDebugResume->setVisible(true);
    ui->btnRunCode->setVisible(true);

    ui->labelSyntaxTree->setVisible(false);
    ui->treeDisplay->setVisible(false);

    ui->labelMonitor->setVisible(true);
    ui->monitorDisplay->setVisible(true);
    ui->labelBreakPoints->setVisible(true);
    ui->breakPointsDisplay->setVisible(true);
}

void MainWindow::setUIExitDebugMode(){
    isDebugMode = false;
    ui->btnClearCode->setVisible(true);
    ui->btnLoadCode->setVisible(true);
    ui->btnDebugMode->setVisible(true);

    ui->btnExitDebugMode->setVisible(false);
    ui->btnDebugResume->setVisible(false);

    ui->labelSyntaxTree->setVisible(true);
    ui->treeDisplay->setVisible(true);

    ui->labelMonitor->setVisible(false);
    ui->monitorDisplay->setVisible(false);
    ui->labelBreakPoints->setVisible(false);
    ui->breakPointsDisplay->setVisible(false);

    ui->monitorDisplay->clear();
    ui->breakPointsDisplay->clear();
    breakpoints.clear();  // 清空断点集合
    context.clear();      // 清空变量上下文（需要在 EvaluationContext 类中添加 clear 方法）
    currentDebugStmt = nullptr;  // 重置当前调试语句
}

void MainWindow::debugResume() {
    if(!isDebugMode || currentDebugStmt == nullptr) {
        return;
    }

    executeDebugCode(currentDebugStmt);
}

void MainWindow::clearAll()
{
    Program->clear();
    delete Program;
    Program = new program;
    Tokenizer->varUsageCount.clear();

    ui->CodeDisplay->clear();
    ui->textBrowser->clear();
    ui->treeDisplay->clear();
}

void MainWindow::load_code()
{
    clearAll();
    QString sourceFile = QFileDialog::getOpenFileName(this, "选择要加载的文件");
    if(sourceFile.isEmpty()) return;

    QFile inputFile(sourceFile);
    if(inputFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->CodeDisplay->clear();
        QTextStream fileStream(&inputFile);

        while(!fileStream.atEnd()){
            QString codeLine = fileStream.readLine();
            if(codeLine.isEmpty()) continue;

            int spaceCount = 0;
            while(spaceCount < codeLine.length() &&
                   (codeLine[spaceCount] == QLatin1Char(' ') ||
                    codeLine[spaceCount] == QLatin1Char('\0') ||
                    codeLine[spaceCount] == QLatin1Char('\n'))) {
                spaceCount++;
            }
            if(spaceCount >= codeLine.length()) continue;

            codeLineEdit_return(codeLine);
        }
        inputFile.close();
    }
}
void MainWindow::executeDebugCode(statement* stmt) {
    currentDebugStmt = stmt;

    while(currentDebugStmt != nullptr) {
        QString content = currentDebugStmt->content;

        // 检查是否遇到断点
        if(breakpoints.contains(currentDebugStmt->line_num)) {
            updateMonitorDisplay();
            if(currentDebugStmt != stmt) { // 如果不是刚开始执行的那条语句
                return;  // 在断点处暂停执行
            }
        }

        if(content.section(" ", 1, 1) == "END") {
            break;
        }
        else if(content.section(" ", 1, 1) == "REM") {
            currentDebugStmt = currentDebugStmt->next;
            continue;
        }
        else if(content.section(" ", 1, 1) == "LET") {
            QString con = content.section(" ", 2, -1);
            QList<QString> _tokens = Tokenizer->tokenize(con);
            Expression* _tree = Parser->parseExpression(_tokens);
            try {
                _tree->eval(context);
            } catch (QString error) {
                ui->textBrowser->append("Error at line " +
                                        QString::number(currentDebugStmt->line_num) + ": " + error);
                return;
            }
        }
        else if(content.section(" ", 1, 1) == "PRINT") {
            QString con = content.section(" ", 2, -1);
            QList<QString> _tokens = Tokenizer->tokenize(con);
            Expression* _tree = Parser->parseExpression(_tokens);
            try {
                int result = _tree->eval(context);
                ui->textBrowser->append(QString::number(result));
            } catch (QString error) {
                ui->textBrowser->append("Error at line " +
                                        QString::number(currentDebugStmt->line_num) + ": " + error);
                return;
            }
        }
        else if(content.section(" ", 1, 1) == "INPUT") {
            ui->cmdLineEdit->setText("?");
            input_name = content.section(" ", 2, -1);
            input_positi = currentDebugStmt;
            return;
        }
        else if(content.section(" ", 1, 1) == "GOTO") {
            int line = content.section(" ", 2, 2).toInt();
            statement* target = Program->find(line);
            if(target) {
                currentDebugStmt = target;
                continue;
            }
        }
        else if(content.section(" ", 1, 1) == "IF") {
            QString condition = content.section(" ", 2, -3);
            if(judge_if(condition)) {
                int line = content.section(" ", -1, -1).toInt();
                statement* target = Program->find(line);
                if(target) {
                    currentDebugStmt = target;
                    continue;
                }
            }
        }

        currentDebugStmt = currentDebugStmt->next;
    }

    // 程序执行完毕
    currentDebugStmt = nullptr;
    updateMonitorDisplay();
}

/*void MainWindow::executeDebugCode(statement* stmt) {
    currentDebugStmt = stmt;

    while(currentDebugStmt != nullptr) {
        QString content = currentDebugStmt->content;

        // 检查是否遇到断点
        if(breakpoints.contains(currentDebugStmt->line_num)) {
            updateMonitorDisplay();
            if(currentDebugStmt != stmt) { // 如果不是刚开始执行的那条语句
                return;  // 在断点处暂停执行
            }
        }

        CommandType cmdType = parseCommandType(content);

        if(cmdType == CommandType::END) {
            break;
        }

        if(commandHandlers.contains(cmdType)) {
            bool continueExecution = commandHandlers[cmdType](currentDebugStmt);
            if(!continueExecution) {
                return;
            }
        }

        currentDebugStmt = currentDebugStmt->next;
    }

    // 程序执行完毕
    currentDebugStmt = nullptr;
    updateMonitorDisplay();
}*/

void MainWindow::updateMonitorDisplay() {
    ui->monitorDisplay->clear();
    QStringList variables = context.getAllVariables();
    for(const QString& var : variables) {
        if(context.isDefined(var)) {
            QString varInfo = var + " = " + QString::number(context.getValue(var));
            ui->monitorDisplay->append(varInfo);
        }
    }
}
void MainWindow::codeLineEdit_return(QString str)
{
    // 获取输入命令
    QString cmd;
    if(str == "") cmd = ui->cmdLineEdit->text();
    else cmd = str;

    if(isDebugMode) {
        if(cmd.startsWith("ADD ")) {
            bool ok;
            int line = cmd.mid(4).toInt(&ok);
            if(ok && Program->find(line)) {
                breakpoints.insert(line);
                ui->breakPointsDisplay->append(QString::number(line));
            } else {
                ui->textBrowser->append("Invalid line number for breakpoint");
            }
            ui->cmdLineEdit->clear();
            return;
        }
        else if(cmd.startsWith("DELETE ")) {  // 添加对 DELETE 命令的处理
            bool ok;
            int line = cmd.mid(7).toInt(&ok);
            if(ok) {
                if(breakpoints.contains(line)) {
                    breakpoints.remove(line);
                    // 更新断点显示
                    ui->breakPointsDisplay->clear();
                    for(int bp : breakpoints) {
                        ui->breakPointsDisplay->append(QString::number(bp));
                    }
                    ui->textBrowser->append("Breakpoint at line " + QString::number(line) + " removed");
                } else {
                    ui->textBrowser->append("No breakpoint at line " + QString::number(line));
                }
            } else {
                ui->textBrowser->append("Invalid line number for breakpoint deletion");
            }
            ui->cmdLineEdit->clear();
            return;
        }
    }

    if(cmd.left(1) == "?"){
        QString tmp_in_num = "";
        tmp_in_num += cmd.mid(1);

        bool in_flag = false;
        int _in_num = tmp_in_num.toInt(&in_flag);
        if(!in_flag){
            ui->cmdLineEdit->clear();
            ui->textBrowser->append("Invaild input,no number!");

            return;
        }
        context.setValue(input_name, _in_num);
        if(if_in_dir == 0) run_code(context, input_positi->next);
        else if_in_dir = 0;

        ui->cmdLineEdit->clear();

        return;
    }

    // 对五个控制命令的处理
    if(cmd == nullptr) return;
    if(cmd == "QUIT") this->close();
    if(cmd == "CLEAR") clearAll();
    if(cmd == "LOAD") load_code();
    if(cmd == "RUN") run_code(context, Program->head->next);
    if(cmd == "HELP"){
        clearAll();
        QString help = "";
        help += "支持语句命令：REM, LET, PRINT, INPUT, GOTO, IF, END; RUN, LOAD, CLEAR, HELP, QUIT\n";
        help += "调试模式下ADD DELETE增删代码\n";
        help += "仅支持在输入区进行输入，回车后输入一行命令\n";
        help += "不支持在其他区进行操作，若要修改已输入语句需重新写一行覆盖\n";

        ui->treeDisplay->append(help);
    }

    // 对无行号直接执行语句的处理
    if(cmd.section(" ", 0, 0) == "INPUT"){
        ui->cmdLineEdit->clear();
        ui->CodeDisplay->append(cmd);

        ui->cmdLineEdit->setText("?");
        input_name = cmd.section(" ", 1, 1);
        if_in_dir = 1;

        run_code(context, Program->head->next);
        return;
    }

    if(cmd.section(" ", 0, 0) == "LET"){
        QString tmp = cmd.section(" ", 1, -1);

        input_stmt->next = new statement(cmd, nullptr);
        input_stmt = input_stmt->next;

    }

    if(cmd.section(" ", 0, 0) == "PRINT"){
        run_code(context, Program->head->next);

        // 表达式树
        QString tmp = cmd.section(" ", 1, -1);
        QList<QString> _tokens;
        Expression* _tree;

        _tokens.clear();
        _tokens = Tokenizer->tokenize(tmp);
        _tree = Parser->parseExpression(_tokens);

        int res;
        try {
            res = _tree->eval(context);
        } catch (QString error) {
            QString error_warn = "";
            error_warn += error;
            ui->textBrowser->append(error_warn);
            return;
        }

        QString output = QString::number(res, 10);
        ui->textBrowser->append(output);
    }

    bool flag = false;
    cmd.section(" ", 0, 0).toInt(&flag);
    QString com_name = cmd.section(" ", 0, 0);
    if(flag){
        int input_line_num = cmd.section(" ", 0, 0).toInt();
        QString cont = cmd.section(" ", 0, -1);
        Program->insert_stmt(cont, input_line_num);
    }

    else if(com_name != "INPUT" && com_name != "PRINT" && com_name != "LET" && com_name != "HELP" && com_name != "RUN" && com_name != "LOAD" && com_name != "CLEAR"){
        QString error = "";
        error += "Invaild direct command!";
        ui->textBrowser->append(error);
        ui->cmdLineEdit->clear();
        return;
    }

    ui->cmdLineEdit->clear();
    ui->CodeDisplay->append(cmd);
}

bool MainWindow::judge_if(QString str){
    QString if_operator;

    if(str.contains("=")){
        if_operator = "=";

        QList<QString> left_tokens;
        Expression* left_exp;
        left_tokens.clear();
        left_tokens = Tokenizer->tokenize(str.section(if_operator, 0, 0));

        left_exp = Parser->parseExpression(left_tokens);
        int left_num;
        left_num = left_exp->eval(context);


        QList<QString> right_tokens;
        Expression* right_exp;
        right_tokens.clear();
        right_tokens = Tokenizer->tokenize(str.section(if_operator, 1, 1));

        right_exp = Parser->parseExpression(right_tokens);
        int right_num;
        right_num = right_exp->eval(context);

        if(left_num == right_num) return true;
        else return false;
    }

    if(str.contains("<")){
        if_operator = "<";

        QList<QString> left_tokens;
        Expression* left_exp;
        left_tokens.clear();
        left_tokens = Tokenizer->tokenize(str.section(if_operator, 0, 0));

        left_exp = Parser->parseExpression(left_tokens);
        int left_num;
        left_num = left_exp->eval(context);


        QList<QString> right_tokens;
        Expression* right_exp;
        right_tokens.clear();
        right_tokens = Tokenizer->tokenize(str.section(if_operator, 1, 1));

        right_exp = Parser->parseExpression(right_tokens);
        int right_num;
        right_num = right_exp->eval(context);

        if(left_num < right_num) return true;
        else return false;
    }

    if(str.contains(">")){
        if_operator = ">";

        QList<QString> left_tokens;
        Expression* left_exp;
        left_tokens.clear();
        left_tokens = Tokenizer->tokenize(str.section(if_operator, 0, 0));

        left_exp = Parser->parseExpression(left_tokens);
        int left_num;
        left_num = left_exp->eval(context);


        QList<QString> right_tokens;
        Expression* right_exp;
        right_tokens.clear();
        right_tokens = Tokenizer->tokenize(str.section(if_operator, 1, 1));

        right_exp = Parser->parseExpression(right_tokens);
        int right_num;
        right_num = right_exp->eval(context);

        if(left_num > right_num) return true;
        else return false;
    }

    else return false;
}

void MainWindow::run() {
    if(!isDebugMode) {
        // 正常模式运行
        run_code(context, Program->head->next);
    } else {
        // 调试模式运行
        executeDebugCode(Program->head->next);
    }
}

QString MainWindow::syntax_behind(QString &out_tree, Expression *node, int num, EvaluationContext &context){
    num++;
    if(node != nullptr){
        if(node->getOperator() != " "){
            out_tree += node->getOperator();
            out_tree += "\n";
            for(int j = 0; j < num; j++) out_tree += "    ";
            Expression* tmp = node;

            while(tmp->getLHS()->getOperator() != " " || tmp->getRHS()->getOperator() != " "){
                if(tmp->getLHS()->getOperator() != " "){
                    out_tree += tmp->getLHS()->getOperator();
                    out_tree += "\n";
                    for(int j = 0; j < num; j++) out_tree += "    ";

                    if(tmp->getRHS()->getIdentifierName() != " "){
                        out_tree += tmp->getRHS()->getIdentifierName();
                    }
                    else out_tree += QString::number(tmp->getRHS()->eval(context));
                    out_tree += "\n";
                    num++;
                    for(int j = 0; j < num; j++) out_tree += "    ";

                    tmp = tmp->getLHS();
                    continue;
                }

                else if(tmp->getRHS()->getOperator() != " "){
                    if(tmp->getLHS()->getIdentifierName() != " "){
                        out_tree += tmp->getLHS()->getIdentifierName();
                    }
                    else out_tree += QString::number(tmp->getLHS()->eval(context));
                    out_tree += "\n";
                    for(int j = 0; j < num; j++) out_tree += "    ";

                    out_tree += tmp->getRHS()->getOperator();
                    out_tree += "\n";
                    num++;
                    for(int j = 0; j < num; j++) out_tree += "    ";

                    tmp = tmp->getRHS();
                    continue;
                }
            }

            if(tmp->getLHS()->getIdentifierName() != " "){
                out_tree += tmp->getLHS()->getIdentifierName();
                out_tree += "\n";
                for(int j = 0; j < num; j++) out_tree += "    ";
            }
            if(tmp->getLHS()->getIdentifierName() == " "){
                out_tree += QString::number(tmp->getLHS()->eval(context));
                out_tree += "\n";
                for(int j = 0; j < num; j++) out_tree += "    ";
            }

            if(tmp->getRHS()->getIdentifierName() != " "){
                out_tree += tmp->getRHS()->getIdentifierName();
            }
            if(tmp->getRHS()->getIdentifierName() == " ") out_tree += QString::number(tmp->getRHS()->eval(context));
        }

        else if(node->getIdentifierName() != " "){
            out_tree += node->getIdentifierName();
        }
        else {
            int res = node->eval(context);
            QString _res = QString::number(res);
            out_tree += _res;
        }
    }
    return out_tree;
}

void MainWindow::run_code(EvaluationContext &context, statement* current_stmt)
{
    ui->treeDisplay->clear();

    statement* cur = current_stmt;
    statement* cur1 = current_stmt;

    while(cur != nullptr){
        QString content = cur->content;

        if(content.section(" ", 1, 1) == "END") break;

        else if(content.section(" ", 1, 1) == "REM"){
            cur = cur->next;
            continue;
        }

        else if(content.section(" ", 1, 1) == "LET"){
            QString con = content.section(" ", 2, -1);
            QString tc = con.remove(QChar(' '), Qt::CaseInsensitive);
            bool flag = false;
            tc.section("=", 0, 0).toInt(&flag);
            if(flag){
                cur->if_error = 1;

                QString error = "";
                error += QString::number(cur->line_num);
                error += ", No identifier to be assigned";
                ui->textBrowser->append(error);

                cur = cur->next;
                continue;
            }

            QList<QString> _tokens;
            Expression* _tree;
            _tokens.clear();
            _tokens = Tokenizer->tokenize(con);
            _tree = Parser->parseExpression(_tokens);

            try {
                _tree->eval(context);
            } catch (QString error) {
                cur->if_error = 1;

                QString error_warn = "";
                error_warn += QString::number(cur->line_num);
                error_warn += ", ";
                error_warn += error;
                ui->textBrowser->append(error_warn);

                cur = cur->next;
                continue;
            }

            cur->trigger_num++;
            cur = cur->next;
            continue;
        }

        else if(content.section(" ", 1, 1) == "PRINT"){
            QString con = content.section(" ", 2, -1);
            QList<QString> _tokens;
            Expression* _tree;
            _tokens.clear();
            _tokens = Tokenizer->tokenize(con);
            _tree = Parser->parseExpression(_tokens);

            int res;
            try {
                res = _tree->eval(context);
            } catch (QString error) {
                cur->if_error = 1;

                QString error_warn = "";
                error_warn += QString::number(cur->line_num);
                error_warn += ", ";
                error_warn += error;
                ui->textBrowser->append(error_warn);

                cur = cur->next;
                continue;
            }

            QString output = QString::number(res, 10);
            ui->textBrowser->append(output);

            cur->trigger_num++;

            cur = cur->next;
            continue;
        }

        else if(content.section(" ", 1, 1) == "INPUT"){
            ui->cmdLineEdit->setText("?");
            input_name = content.section(" ", 2, -1);
            input_positi = cur;
            return;
        }

        else if(content.section(" ", 1, 1) == "GOTO"){
            cur->trigger_num++;

            int jump_line;
            bool ok;
            content.section(" ", 2, 2).toInt(&ok);

            if(ok){
                jump_line = content.section(" ", 2, 2).toInt();
                statement* tmp = Program->head->next;

                while(tmp != nullptr){
                    if(tmp->content.section(" ", 0, 0).toInt() == jump_line){
                        break;
                    }
                    tmp = tmp->next;
                }
                if(tmp == nullptr){
                    cur->if_error = 1;

                    QString error = "";
                    error += QString::number(cur->line_num);
                    error += ", GOTO error!Line NOT Found!";
                    ui->textBrowser->append(error);
                }

                else{
                    cur = tmp;
                    continue;
                }
            }
            else{
                cur->if_error = 1;

                QString error = "";
                error += QString::number(cur->line_num);
                error += ", No line number in input!";
                ui->textBrowser->append(error);
            }

            cur = cur->next;
            continue;
        }

        else if(content.section(" ", 1, 1) == "IF"){
            QString if_stat = content.section(" ", 2, -3);

            bool flag;

            flag = judge_if(if_stat);

            if(flag){
                cur->if_tri_true++;
                QString then_line_num = content.section(" ", -1, -1);
                int des_line = then_line_num.toInt();

                statement* tmp = Program->head->next;
                while(tmp != nullptr){
                    if(tmp->content.section(" ", 0, 0).toInt() == des_line){
                        break;
                    }
                    tmp = tmp->next;
                }
                if(tmp == nullptr){
                    QString error = "";
                    error += "Invaild line number!";
                    ui->textBrowser->append(error);
                }
                else{
                    cur = tmp;
                    continue;
                }
            }
            else cur->if_tri_false++;

            cur = cur->next;
            continue;
        }

        else{
            QString error = "";
            error += content.section(" ", 0, 0);
            error += ", invaild command!";
            ui->textBrowser->append(error);

            cur = cur->next;
            continue;
        }

    }

    /*while(cur != nullptr) {
        CommandType cmdType = parseCommandType(cur->content);
        if(cmdType == CommandType::END) break;

        if(commandHandlers.contains(cmdType)) {
            bool continueExecution = commandHandlers[cmdType](cur);
            if(!continueExecution) {
                if(cmdType == CommandType::INPUT) {
                    return;
                }
                cur = cur->next;
                continue;
            }
        } else {
            handleError(cur, "Invalid command");
        }

        cur = cur->next;
    }*/

    while(cur1 != nullptr){
        cur1->syntax_tree.clear();
        QString content1 = cur1->content;
        if(content1.section(" ", 1, 1) == "END") {
            cur1->syntax_tree += content1.section(" ", 0, 0);  // 行号
            cur1->syntax_tree += " END\n";
        }
        if(content1.section(" ", 1, 1) == "REM") {
            cur1->syntax_tree += content1.section(" ", 0, 0);  // 行号
            cur1->syntax_tree += " REM";

            // 获取注释内容（从REM后面开始到行尾）
            QString comment = content1.section(" ", 2, -1);
            if(!comment.isEmpty()) {
                cur1->syntax_tree += "\n    ";
                cur1->syntax_tree += comment;
            }
        }
        if(content1.section(" ", 1, 1) == "GOTO"){
            if(cur1->if_error){
                cur1->syntax_tree += content1.section(" ", 0, 0);
                cur1->syntax_tree += " error!";
                cur1->syntax_tree += "\n";
            }

            else{
                cur1->syntax_tree += content1.section(" ", 0, 0);
                cur1->syntax_tree += " GOTO ";
                cur1->syntax_tree += "\n";
                cur1->syntax_tree += "    ";
                cur1->syntax_tree += content1.section(" ", -1, -1);
            }
        }

        else if(content1.section(" ", 1, 1) == "LET"){
            if(cur1->if_error){
                cur1->syntax_tree += content1.section(" ", 0, 0);
                cur1->syntax_tree += " error!";
                cur1->syntax_tree += "\n";
            }

            else{
                QString con1 = content1.section(" ", 2, -1);
                QList<QString> _tokens1;
                Expression* _tree1;
                _tokens1.clear();
                _tokens1 = Tokenizer_tree->tokenize(con1);
                _tree1 = Parser->parseExpression(_tokens1);

                QString to_get_syn = con1.remove(QChar(' '), Qt::CaseInsensitive);
                cur1->syntax_tree += content1.section(" ", 0, 0);
                cur1->syntax_tree += " LET\n";
                cur1->syntax_tree += "    =\n";
                cur1->syntax_tree += "    ";
                cur1->syntax_tree += to_get_syn.section("=", 0, 0);
                cur1->syntax_tree += "\n";
                cur1->syntax_tree += "    ";
                Expression* node1 = _tree1->getRHS();
                syntax_behind(cur1->syntax_tree, node1, 1, context);
            }
        }

        else if(content1.section(" ", 1, 1) == "PRINT"){
            if(cur1->if_error){
                cur1->syntax_tree += content1.section(" ", 0, 0);
                cur1->syntax_tree += " error!";
                cur1->syntax_tree += "\n";
            }

            else{
                QString con1 = content1.section(" ", 2, -1);
                QList<QString> _tokens1;
                Expression* _tree1;
                _tokens1.clear();
                _tokens1 = Tokenizer_tree->tokenize(con1);
                _tree1 = Parser->parseExpression(_tokens1);
                cur1->syntax_tree += content1.section(" ", 0, 0);
                cur1->syntax_tree += " PRINT\n";
                cur1->syntax_tree += "    ";
                Expression* node1 = _tree1;
                syntax_behind(cur1->syntax_tree, node1, 1, context);
            }
        }

        else if(content1.section(" ", 1, 1) == "IF"){
            QString to_f = content1.section(" ", 2, -3);

            QString to_get_syn = to_f.remove(QChar(' '), Qt::CaseInsensitive);
            cur1->syntax_tree += content1.section(" ", 0, 0);
            cur1->syntax_tree += " IF THEN\n";
            cur1->syntax_tree += "    ";
            if(to_get_syn.contains("=")){

                cur1->syntax_tree += to_get_syn.section("=", 0, 0);
                cur1->syntax_tree += "\n    =\n    ";
                cur1->syntax_tree += to_get_syn.section("=", 1, 1);
                cur1->syntax_tree += "\n    ";
            }
            if(to_get_syn.contains(">")){

                cur1->syntax_tree += to_get_syn.section(">", 0, 0);
                cur1->syntax_tree += "\n    >\n    ";
                cur1->syntax_tree += to_get_syn.section(">", 1, 1);
                cur1->syntax_tree += "\n    ";
            }
            if(to_get_syn.contains("<")){

                cur1->syntax_tree += to_get_syn.section("<", 0, 0);
                cur1->syntax_tree += "\n    <\n    ";
                cur1->syntax_tree += to_get_syn.section("<", 1, 1);
                cur1->syntax_tree += "\n    ";
            }
            cur1->syntax_tree += content1.section(" ", -1, -1);
        }

        ui->treeDisplay->append(cur1->syntax_tree);
        cur1 = cur1->next;
    }
    return;
}
