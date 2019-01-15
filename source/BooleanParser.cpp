#ifndef BOOLEANPARSERCPP
#define BOOLEANPARSERCPP
#include <iostream>
#include "Expression.h"
#include "BooleanParser.h"
using namespace std;


Expression* parseBooleanExpression(string booleanExpression){
    Expression* parsed_expression = parseBooleanExpressionHelper(booleanExpression);
    if(parsed_expression == NULL){
        return NULL;
    }
    parsed_expression->clean();
    return parsed_expression;
}
Expression* parseBooleanExpressionHelper(string booleanExpression){ // recursive function to parse a boolean expression
    // Expressions will be nested to some unknown level, so we will first determine each statement's depth
    booleanExpression.erase(std::remove_if(booleanExpression.begin(), booleanExpression.end(), ::isspace), booleanExpression.end());
    Expression* mainConnective = new Expression();
    static const char disjunction_arr[] = {'|','+'};
    vector<char> disjunctions(disjunction_arr, disjunction_arr + sizeof(disjunction_arr) / sizeof(disjunction_arr[0]) );
    static const char conjunction_arr[] = {'^','&','*'};
    vector<char> conjunctions(conjunction_arr, conjunction_arr + sizeof(conjunction_arr) / sizeof(conjunction_arr[0]) );
    static const char negation_arr[] = {'~','!','-'};
    vector<char> negations(negation_arr, negation_arr + sizeof(negation_arr) / sizeof(negation_arr[0]) );
    vector<string> implications;
    implications.push_back("->");
    implications.push_back("_>");
    vector<string> dbl_implications;
    dbl_implications.push_back("<->");
    dbl_implications.push_back("<_>");
    //static const char negation_arr_postfix[] = {'\''};
    //vector<char> negations_postfix(negation_arr_postfix, negation_arr_postfix + sizeof(negation_arr_postfix) / sizeof(negation_arr_postfix[0]) );

    int depth = 0; // keeps track of how deep we are in the expression so we know where the main connective is
    bool depth_reached_zero = false;
    while(booleanExpression[0] == '(' && booleanExpression[booleanExpression.length()-1]==')' && depth_reached_zero == false){
        depth = 1;
        depth_reached_zero = false;
        for(int i = 1; i < booleanExpression.length()-2;i++){
            if(booleanExpression[i] == '('){ // if we see a begin paren, we are going deeper
                depth++;
            }
            if(booleanExpression[i] == ')'){ //  if we see an end paren, we are going out
                depth--;
            }
            if(depth == 0){
                depth_reached_zero = true;
            }
        }
        if(!depth_reached_zero){
            booleanExpression = booleanExpression.substr(1,booleanExpression.length()-2);
        }
    }
    depth = 0;
    // now we should have a statement without parens surrounding it
    if(booleanExpression.length()==1){ // if the expression is merely an atomic statement
        mainConnective->Expression_type = Expression::ATOMIC;
        mainConnective->symbol = booleanExpression[0]; // make the main connective an atomic symbol
        return mainConnective;
    }
    for(int i = 0; i < booleanExpression.length();i++){
        if(depth == 0){
            if(find(disjunctions.begin(),disjunctions.end(),booleanExpression[i]) != disjunctions.end()){
                mainConnective->Expression_type = Expression::OR;
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(0,i))); // create first operand
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(i+1,booleanExpression.length())));
                if(mainConnective->operands[0] == NULL){return NULL;}
                if(mainConnective->operands[1] == NULL){return NULL;}
                // create second operand
                return mainConnective;
            }
        }
        if(booleanExpression[i] == '('){ // if we see a begin paren, we are going deeper
            depth++;
        }
        if(booleanExpression[i] == ')'){ //  if we see an end paren, we are going out
            depth--;
        }
    }
    for(int i = 0; i < booleanExpression.length();i++){
        if(depth == 0){
            if(find(disjunctions.begin(),disjunctions.end(),booleanExpression[i]) != disjunctions.end()){
                mainConnective->Expression_type = Expression::OR;
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(0,i))); // create first operand
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(i+1,booleanExpression.length())));
                if(mainConnective->operands[0] == NULL){return NULL;}
                if(mainConnective->operands[1] == NULL){return NULL;}
                // create second operand
                return mainConnective;
            }
        }
        if(booleanExpression[i] == '('){ // if we see a begin paren, we are going deeper
            depth++;
        }
        if(booleanExpression[i] == ')'){ //  if we see an end paren, we are going out
            depth--;
        }
    }
    depth = 0;
    for(int i = 0; i < booleanExpression.length();i++){
        if(depth == 0){
            if(find(conjunctions.begin(),conjunctions.end(),booleanExpression[i]) != conjunctions.end()){
                mainConnective->Expression_type = Expression::AND;
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(0,i))); // create first operand
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(i+1,booleanExpression.length())));
                if(mainConnective->operands[0] == NULL){return NULL;}
                if(mainConnective->operands[1] == NULL){return NULL;}
                // create second operand
                return mainConnective;
            }
        }
        if(booleanExpression[i] == '('){ // if we see a begin paren, we are going deeper
            depth++;
        }
        if(booleanExpression[i] == ')'){ //  if we see an end paren, we are going out
            depth--;
        }
    }
    depth = 0;
    for(int i = 0; i < booleanExpression.length();i++){
        if(depth == 0){
            if(find(dbl_implications.begin(),dbl_implications.end(),booleanExpression.substr(i,3)) != dbl_implications.end()){
                mainConnective->Expression_type = Expression::OR;
                Expression* new_expression1 = parseBooleanExpression(booleanExpression.substr(0,i));
                Expression* new_expression2 = new Expression();
                Expression* new_expression3 = parseBooleanExpression(booleanExpression.substr(i+3,booleanExpression.length()));
                Expression* new_expression4 = new Expression();
                if(new_expression1 == NULL or new_expression3 == NULL){return NULL;}
                *(new_expression2) = *(new_expression1);
                *(new_expression4) = *(new_expression3);
                new_expression2->negate_in_place(false);
                new_expression4->negate_in_place(false);
                Expression* new_outer_expression1 = new Expression();
                new_outer_expression1->Expression_type = Expression::AND;
                new_outer_expression1->operands.push_back(new_expression1);
                new_outer_expression1->operands.push_back(new_expression3);
                Expression* new_outer_expression2 = new Expression();
                new_outer_expression2->Expression_type = Expression::AND;
                new_outer_expression2->operands.push_back(new_expression2);
                new_outer_expression2->operands.push_back(new_expression4);
                mainConnective->operands.push_back(new_outer_expression1);
                mainConnective->operands.push_back(new_outer_expression2);
                if(mainConnective->operands[0] == NULL){return NULL;}
                if(mainConnective->operands[1] == NULL){return NULL;}
                // create second operand
                return mainConnective;
            }
        }
        if(booleanExpression[i] == '('){ // if we see a begin paren, we are going deeper
            depth++;
        }
        if(booleanExpression[i] == ')'){ //  if we see an end paren, we are going out
            depth--;
        }
    }
    depth = 0;
    for(int i = 0; i < booleanExpression.length();i++){
        if(depth == 0){
            if(find(implications.begin(),implications.end(),booleanExpression.substr(i,2)) != implications.end()){
                mainConnective->Expression_type = Expression::OR;
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(0,i))); // create first operand
                mainConnective->operands[mainConnective->operands.size()-1]->negate_in_place(false);
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(i+2,booleanExpression.length())));
                if(mainConnective->operands[0] == NULL){return NULL;}
                if(mainConnective->operands[1] == NULL){return NULL;}
                // create second operand
                return mainConnective;
            }
        }
        if(booleanExpression[i] == '('){ // if we see a begin paren, we are going deeper
            depth++;
        }
        if(booleanExpression[i] == ')'){ //  if we see an end paren, we are going out
            depth--;
        }
    }
    depth = 0;
    if(find(negations.begin(),negations.end(),booleanExpression[0]) != negations.end()){
        mainConnective->Expression_type = Expression::NOT;
        mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(1,booleanExpression.length())));
        if(mainConnective->operands[0] == NULL){return NULL;}
        // create only operand
        return mainConnective;
    }
    cerr << "Error: invalid function syntax" << endl;
    return NULL;
}
#else
#endif
