#include <cstdlib>
#include "Expression.h"
Expression* parseBooleanExpressionHelper(string booleanExpression);
Expression* parseBooleanExpression(string booleanExpression);
Expression* parseExpressionXML(string path);
void expressionToXML(Expression* exp, string path);
