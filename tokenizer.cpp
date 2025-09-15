#include "tokenizer.h"

CodeTokenizer::CodeTokenizer()
{

}

QList<QString> CodeTokenizer::tokenize(QString input_exp)
{
    tokenList.clear();
    // 储存上一个字符末位置
    int loc = -1;
    QString tmp = input_exp.remove(QChar(' '), Qt::CaseInsensitive);
    for(int i = 0; i < tmp.length(); i++){
        // flag1对应只含一个字符，flag2对应"**"，flag3对应"MOD"
        bool flag1 = false;
        bool flag2 = false;
        bool flag3 = false;

        if(tmp[i] == QLatin1Char('+') || tmp[i] == QLatin1Char('-') ||
            (tmp[i] == QLatin1Char('*') && (i + 1 < tmp.length() && tmp[i + 1] != QLatin1Char('*'))) ||
            tmp[i] == QLatin1Char('/')) {
            flag1 = true;
        }
        if(tmp[i] == QLatin1Char('=') || tmp[i] == QLatin1Char('(') ||
            tmp[i] == QLatin1Char(')')) {
            flag1 = true;
        }

        if(tmp[i] == QLatin1Char('*') && i + 1 < tmp.length() &&
            tmp[i + 1] == QLatin1Char('*')) {
            flag2 = true;
        }

        if(tmp[i] == QLatin1Char('M') && i + 2 < tmp.length() &&
            tmp[i + 1] == QLatin1Char('O') && tmp[i + 2] == QLatin1Char('D')) {
            flag3 = true;
        }

        if(flag1){
            // 截取符号间字符串
            QString t = tmp.mid(loc + 1, i - loc - 1);

            bool ok = varUsageCount.contains(t);
            if(ok){
                int num = varUsageCount.value(t) + 1;
                varUsageCount[t] = num;
            }
            else varUsageCount[t] = 1;

            tokenList.append(t);
            tokenList.append(QString(tmp[i]));
            loc = i;
            continue;
        }

        else if(flag2){
            QString t = tmp.mid(loc + 1, i - loc - 1);

            bool ok = varUsageCount.contains(t);
            if(ok){
                int num = varUsageCount.value(t) + 1;
                varUsageCount[t] = num;
            }
            else varUsageCount[t] = 1;

            tokenList.append(t);
            QString t2 = "**";
            tokenList.append(t2);
            loc = i + 1;
            i++;
            continue;
        }

        else if(flag3){
            QString t = tmp.mid(loc + 1, i - loc - 1);

            bool ok = varUsageCount.contains(t);
            if(ok){
                int num = varUsageCount.value(t) + 1;
                varUsageCount[t] = num;
            }
            else varUsageCount[t] = 1;

            tokenList.append(t);
            QString t3 = "MOD";
            tokenList.append(t3);
            loc = i + 2;
            i = i + 2;
            continue;
        }
    }
    if(loc != -1){
        QString x = tmp.mid(loc + 1, tmp.length() - 1 - loc);

        bool ok = varUsageCount.contains(x);
        if(ok){
            int num = varUsageCount.value(x) + 1;
            varUsageCount[x] = num;
        }
        else varUsageCount[x] = 1;

        tokenList.append(x);
    }
    else{
        bool ok = varUsageCount.contains(tmp);
        if(ok){
            int num = varUsageCount.value(tmp) + 1;
            varUsageCount[tmp] = num;
        }

        tokenList.append(tmp);
    }

    return tokenList;
}

QMap<QString, int> CodeTokenizer::getVarUsage()
{
    return varUsageCount;
}
