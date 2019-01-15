#include "expressionserializer.h"

Expression* ExpressionSerializer::parseExpressionFile(QString path){
    QFile inFile(path);
    inFile.open(QIODevice::ReadOnly);
    QTextStream instream(&inFile);
    return parseExpressionFileHelper(instream);
}
Expression* ExpressionSerializer::parseExpressionFileHelper(QTextStream &inStream){
    QString firstTag;
    Expression* retExpr = new Expression();
    firstTag = inStream.readLine().remove("\n").remove("\r").remove("\t");
    if(firstTag == "</AND>"){
        return NULL;
    }
    if(firstTag == "</OR>"){
        return NULL;
    }
    if(firstTag == "</NOT>"){
        return NULL;
    }
    if(firstTag == "<AND>"){
        retExpr->Expression_type = Expression::AND;
        while(true){
            Expression* nextExpr = parseExpressionFileHelper(inStream);
            if(nextExpr == NULL){
                break;
            }
            else{
                retExpr->operands.push_back(nextExpr);
            }
        }
        return retExpr;
    }
    if(firstTag == "<OR>"){
        retExpr->Expression_type = Expression::OR;
        while(true){
            Expression* nextExpr = parseExpressionFileHelper(inStream);
            if(nextExpr == NULL){
                break;
            }
            else{
                retExpr->operands.push_back(nextExpr);
            }
        }
        return retExpr;
    }
    if(firstTag == "<NOT>"){
        retExpr->Expression_type = Expression::NOT;
        while(true){
            Expression* nextExpr = parseExpressionFileHelper(inStream);
            if(nextExpr == NULL){
                break;
            }
            else{
                retExpr->operands.push_back(nextExpr);
            }
        }
        return retExpr;
    }
    if(firstTag.length() == 1){
        retExpr->Expression_type = Expression::ATOMIC;
        retExpr->symbol = firstTag.toStdString()[0];
        return retExpr;
    }
    qDebug() << firstTag;
}

void ExpressionSerializer::expressionToFile(Expression *exp, QString path){
    QFile outFile(path);
    outFile.open(QIODevice::WriteOnly);
    expressionToFileHelper(exp,0,outFile);
    outFile.close();
}

void ExpressionSerializer::expressionToFileHelper(Expression *exp, int depth, QFile& outFile){
    QString tabs = "";
    QTextStream outstream(&outFile);
    for(int i = 0; i < depth; i++){
        tabs.append("\t");
    }
    if(exp->Expression_type == Expression::AND){
        outstream << tabs << "<AND>" << endl;
        for(int i = 0; i < exp->operands.size(); i++){
            expressionToFileHelper(exp->operands[i],depth+1,outFile);
        }
        outstream << tabs << "</AND>" << endl;
    }
    if(exp->Expression_type == Expression::OR){
        outstream << tabs << "<OR>" << endl;
        for(int i = 0; i < exp->operands.size(); i++){
            expressionToFileHelper(exp->operands[i],depth+1,outFile);
        }
        outstream << tabs << "</OR>" << endl;
    }
    if(exp->Expression_type == Expression::NOT){
        outstream << tabs << "<NOT>" << endl;
        expressionToFileHelper(exp->operands[0],depth+1,outFile);
        outstream << tabs << "</NOT>" << endl;
    }
    if(exp->Expression_type == Expression::ATOMIC){
        outstream << tabs << exp->symbol << endl;
    }
}
