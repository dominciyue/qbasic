#include "evalstate.h"
#include <QStringList>

void EvaluationContext::setValue(QString var, int value)
{
    varStorage[var] = value;
}

int EvaluationContext::getValue(QString var)
{
    return varStorage[var];
}

bool EvaluationContext::isDefined(QString var)
{
    return varStorage.count(var);
}

void EvaluationContext::clear()
{
    varStorage.clear();
}

QStringList EvaluationContext::getAllVariables() const {
    QStringList vars;
    for(const auto& pair : varStorage) {
        vars.append(pair.first);
    }
    return vars;
}
