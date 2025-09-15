#ifndef EVALSTATE_H
#define EVALSTATE_H

#include <QString>
#include <iostream>
#include <map>
#include <QStringList>

using namespace std;

class EvaluationContext {
public:
   void setValue(QString var, int value);
   int getValue(QString var);
   bool isDefined(QString var);
   void clear();
   QStringList getAllVariables() const;

private:
   map<QString,int> varStorage;
};

#endif // EVALSTATE_H
