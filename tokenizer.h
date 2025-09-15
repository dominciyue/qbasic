#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QString>
#include <QList>
#include <QMap>

class CodeTokenizer {
public:
    CodeTokenizer();
    QList<QString> tokenize(QString code);
    QMap<QString, int> getVarUsage();
    QMap<QString, int> varUsageCount;

private:
    QList<QString> tokenList;
private:
    bool isBasicOperator(QChar c) {
        return c == '+' || c == '-' || c == '*' || c == '/' ||
               c == '=' || c == '(' || c == ')';
    }

    bool isPartOfSpecialOperator(const QString& exp, int pos) {
        // 检查是否是 ** 或 MOD 的开始
        if(pos + 1 < exp.length() &&
            exp[pos] == '*' && exp[pos + 1] == '*') {
            return true;
        }
        if(pos + 2 < exp.length() &&
            exp.mid(pos, 3).toUpper() == "MOD") {
            return true;
        }
        return false;
    }

    void processToken(const QString& token) {
        tokenList.append(token);

        // 更新变量使用计数
        if(!isBasicOperator(token[0]) && token != "MOD" && token != "**") {
            if(varUsageCount.contains(token)) {
                varUsageCount[token]++;
            } else {
                varUsageCount[token] = 1;
            }
        }
    }
};

#endif
