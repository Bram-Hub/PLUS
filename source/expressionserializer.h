#ifndef EXPRESSIONSERIALIZER_H
#define EXPRESSIONSERIALIZER_H
#include "Expression.h"
#include <QtGui>
#include <QtCore>
class ExpressionSerializer
{
public:
    static Expression* parseExpressionFile(QString path);
    static Expression* parseExpressionFileHelper(QTextStream & inStream);
    static void expressionToFile(Expression *exp, QString path);
    static void expressionToFileHelper(Expression* exp, int depth, QFile & outFile);
};

#endif // EXPRESSIONSERIALIZER_H
