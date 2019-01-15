#ifndef EXPRESSIONCPP
#define EXPRESSIONCPP
#include "Expression.h"
#include <cmath>
#include <numeric>
using namespace std;
Expression::Expression(){
    if (Expression::simplification_rules.size() == 0) {
        Expression::simplification_rules.push_back("double negation");
        Expression::simplification_rules.push_back("identity");
        Expression::simplification_rules.push_back("annihilation");
        Expression::simplification_rules.push_back("contradiction");
        Expression::simplification_rules.push_back("tautology");
        Expression::simplification_rules.push_back("inverse");
        Expression::simplification_rules.push_back("idepotence");
        Expression::simplification_rules.push_back("demorgan");
        Expression::simplification_rules.push_back("absorption");
        Expression::simplification_rules.push_back("reduction");
        //Expression::simplification_rules.push_back("distribution_backwards");
        //Expression::simplification_rules.push_back("distribution_forward");
        Expression::suppress_output = false;
    }
    Expression_type = Expression::ATOMIC;
    symbol = '1'; // set the symbol to a tautology initially
}
// Copy constructor
Expression::Expression(const Expression& existingExpression)
{
    for(int i = 0; i < operands.size(); i++){
        delete operands[i];
    }
    operands.clear();
    for(int i = 0; i < existingExpression.operands.size(); i++){
        Expression* new_exp = new Expression(*(existingExpression.operands[i]));
        operands.push_back(new_exp);
    }
    symbol = existingExpression.symbol;
    Expression_type = existingExpression.Expression_type;
}

Expression& Expression::operator= (const Expression& existingExpression){
    for(int i = 0; i < operands.size(); i++){
        delete operands[i];
    }
    operands.clear();
    for(int i = 0; i < existingExpression.operands.size(); i++){
        Expression* new_exp = new Expression(*(existingExpression.operands[i]));
        operands.push_back(new_exp);
    }
    symbol = existingExpression.symbol;
    Expression_type = existingExpression.Expression_type;
    return *this;
}
Expression::~Expression(){
    for(int i = 0; i < operands.size(); i++){
        delete operands[i];
    }
}
bool Expression::is_atomic_symbol() const
{
    return (operands.size()==0);
}

bool operator==(const Expression& lhs, const Expression& rhs)
{
    if(lhs.operands.size() != rhs.operands.size()){
        return false;
    }
    if(lhs.operands.size() == 0){
        return (lhs.symbol == rhs.symbol);
    }
    if(lhs.Expression_type != rhs.Expression_type){
        return false;
    }
    vector<Expression*> unused_rhs_ops = rhs.operands;
    bool has_match = false;
    for(int i = 0; i < lhs.operands.size(); i++){
        has_match = false;
        for(int j = 0; j < unused_rhs_ops.size(); j++){
            if(*(lhs.operands[i]) == *(unused_rhs_ops[j])){
                has_match = true;
                unused_rhs_ops.erase(unused_rhs_ops.begin()+j);
                j--;
            }
        }
        if(!has_match){
            return false;
        }
    }
    return true;
}
bool equals(const Expression& lhs, const Expression& rhs)
{
    if(lhs.operands.size() != rhs.operands.size()){
        return false;
    }
    if(lhs.operands.size() == 0){
        return (lhs.symbol == rhs.symbol);
    }
    if(lhs.Expression_type != rhs.Expression_type){
        return false;
    }
    for(int i = 0; i < lhs.operands.size(); i++){
        if(!equals(*(lhs.operands[i]),*(rhs.operands[i]))){
            return false;
        }
    }
    return true;
}
bool operator!=(const Expression& lhs, const Expression& rhs)
{
    return !(operator == (lhs,rhs));
}
void printTransformation(const Expression& from , const Expression& to , const string method);
vector<string> Expression::simplification_rules;
bool Expression::suppress_output;
void Expression::simplifyCompletely(bool dnf){
    printHumanReadable();
    vector<Expression*> expressions_expanded;
    bool placeholder_bool;
    while(simplify(expressions_expanded,placeholder_bool,dnf)){printHumanReadable();} // simplify until we can simplify no more
    for(int i = 0; i < expressions_expanded.size(); i++){
        delete expressions_expanded[i];
    }
}
void Expression::expand_to_cnf(){
    printHumanReadable();
    cout << "Expanding" << endl;
    clean();
    while(applyInferenceRule("distribution_forward_disjuncts_only")){
        clean();
        printHumanReadable();
        while(applyInferenceRule("demorgan")){}
        bool applied = true;
        /*while(applied == true){
            applied = false;
            for(int i = 0; i<simplification_rules.size();i++){ // attempt to apply each inference rule until we find one we can apply
                if(applyInferenceRule(Expression::simplification_rules[i])){ // if we successfully applied the rule
                    cout << Expression::simplification_rules[i] << endl;
                    applied = true; // do nothing else, we only want to apply one rule per simplification
                    break;
                }
            }
            clean();
            printHumanReadable();
        }*/
    }
}
void Expression::expand_to_dnf(){
    printHumanReadable();
    cout << "Expanding" << endl;
    clean();
    while(applyInferenceRule("distribution_forward_conjuncts_only")){
        clean();
        printHumanReadable();
        while(applyInferenceRule("demorgan")){}
        bool applied = true;
        /*while(applied == true){
            applied = false;
            for(int i = 0; i<simplification_rules.size();i++){ // attempt to apply each inference rule until we find one we can apply
                if(applyInferenceRule(Expression::simplification_rules[i])){ // if we successfully applied the rule
                    cout << Expression::simplification_rules[i] << endl;
                    applied = true; // do nothing else, we only want to apply one rule per simplification
                    break;
                }
            }
            clean();
            printHumanReadable();
        }*/
    }
}
bool Expression::is_atom_or_negation_of_atom(){
    if(Expression_type == Expression::ATOMIC){
        return true;
    }
    if(Expression_type == Expression::NOT){
        if(operands[0]->Expression_type == Expression::ATOMIC){
            return true;
        }
    }
    return false;
}
bool Expression::is_in_dnf(){
    clean();
    if(is_atom_or_negation_of_atom()){
        return true;
    }
    if(Expression_type == Expression::OR){
        bool is_good = false;
        for(int i = 0; i < operands.size(); i++){
            if(operands[i]->is_atom_or_negation_of_atom()){
                is_good = true;
            }
            if(operands[i]->Expression_type == Expression::AND){
                is_good = true;
                for(int j = 0; j < operands[i]->operands.size(); j++){
                    if(!operands[i]->operands[j]->is_atom_or_negation_of_atom()){
                        return false;
                    }
                }
            }
            if(!is_good){
                return false;
            }
        }
        return true;
    }

    if(Expression_type == Expression::AND){
        for(int j = 0; j < operands.size(); j++){
            if(!operands[j]->is_atom_or_negation_of_atom()){
                return false;
            }
        }
        return true;
    }
    return false;
}
bool Expression::is_in_cnf(){
    clean();
    if(is_atom_or_negation_of_atom()){
        return true;
    }
    if(Expression_type == Expression::AND){
        bool is_good = false;
        for(int i = 0; i < operands.size(); i++){
            if(operands[i]->is_atom_or_negation_of_atom()){
                is_good = true;
            }
            if(operands[i]->Expression_type == Expression::OR){
                is_good = true;
                for(int j = 0; j < operands[i]->operands.size(); j++){
                    if(!operands[i]->operands[j]->is_atom_or_negation_of_atom()){
                        return false;
                    }
                }
            }
            if(!is_good){
                return false;
            }
        }
        return true;
    }

    if(Expression_type == Expression::OR){
        for(int j = 0; j < operands.size(); j++){
            if(!operands[j]->is_atom_or_negation_of_atom()){
                return false;
            }
        }
        return true;
    }
    return false;
}
bool Expression::is_in_nnf(){
    clean();
    if(is_atom_or_negation_of_atom()){
        return true;
    }
    if(Expression_type == Expression::AND || Expression_type == Expression::OR){
        for(int i = 0; i < operands.size(); i++){
            if(!operands[i]->is_in_nnf()){
                return false;
            }
        }
        return true;
    }
    if(Expression_type == Expression::NOT){
        return (operands[0]->Expression_type == Expression::ATOMIC); // always false, but looks better like this
    }
}
bool Expression::simplify(vector<Expression*>& expressions_expanded, bool& expanded, bool dnf){
    clean();
    /* It is necessary that we apply the 'best' inference rule possible, regardless of where it is in the tree */
    for(int i = 0; i<simplification_rules.size();i++){ // attempt to apply each inference rule until we find one we can apply
        if(applyInferenceRule(Expression::simplification_rules[i])){ // if we successfully applied the rule
            cout << Expression::simplification_rules[i] << endl;
            return true; // do nothing else, we only want to apply one rule per simplification
        }
        /* If we are unable to apply a rule at this level, we will go down one level*/
    }
    if(!expanded){
        if(dnf){
            expand_to_dnf();
        }
        else{
            expand_to_cnf();
        }
        expanded = true;
    }
    clean();
    for(int i = 0; i<simplification_rules.size();i++){ // attempt to apply each inference rule until we find one we can apply
        if(applyInferenceRule(Expression::simplification_rules[i])){ // if we successfully applied the rule
            cout << Expression::simplification_rules[i] << endl;
            return true; // do nothing else, we only want to apply one rule per simplification
        }
        /* If we are unable to apply a rule at this level, we will go down one level*/
    }
    /*
    bool found_equal = false;
    for(int j = 0; j < operands.size(); j++){
        reorder_outer_operands();
        for(int k = 0; k < operands[0]->operands.size(); k++){
            reorder(k);
            found_equal = false;
            for(int i = 0; i < expressions_expanded.size(); i++){ // TODO work on reordering 
                if(equals(*(expressions_expanded[i]),*this)){
                    found_equal = true;
                    break;
                }
            }
            if(!found_equal){
                break;
            }
        }
        if(!found_equal){
            break;
        }
    }
    if(!found_equal){
        expressions_expanded.push_back(new Expression(*this));
        if(applyEquivalencePrinciple("distribution_forward")){
            printHumanReadable();
            return true;
        }
    }
    */
    return false; // we are simplified in dnf or cnf depending on dnf variable
}
void Expression::reorder_outer_operands(){
    Expression* temp_ptr = operands[0];
    operands.erase(operands.begin());
    operands.push_back(temp_ptr);
    temp_ptr = NULL;
}
bool Expression::reorder(int operand_to_reorder){
    Expression* temp_ptr = operands[0]->operands[operand_to_reorder];
    operands[0]->operands.erase(operands[0]->operands.begin()+operand_to_reorder);
    operands[0]->operands.push_back(temp_ptr);
    temp_ptr = NULL;
}
bool Expression::applyInferenceRule(string str){
    /* We first attempt to apply the inference rule directly to the current statement*/
    if(applyEquivalencePrinciple(str)){
        return true;
    }
    else{
        if(Expression_type == Expression::ATOMIC){ // if the statement is atomic
            return false;
            // Nothing to do here... we are done
        }
        for(int j = 0; j<operands.size();j++){ // otherwise, try to apply the rule to all children
            if(operands[j]->applyInferenceRule(str)){ // if the rule can be applied to any child
                return true; // we are done
            }
        }
    }
    return false;
}
void Expression::deleteAllOperands(){
    for(int i = 0; i < operands.size(); i++){
        delete operands[i];
    }
    operands.clear();
}
bool Expression::expandExpression(string principle, Expression* exp_to_add){
    if(exp_to_add == NULL){
        exp_to_add = new Expression();
        exp_to_add->Expression_type = Expression::ATOMIC;
        exp_to_add->symbol = 'a';
    }
    if(principle == "double negation"){
        if(Expression_type != Expression::NOT){
            negate_in_place();
            negate_in_place(false); // does not remove the dbl negation
            return true;
        }
        else{
            negate_in_place(false);
            negate_in_place(false); // does not remove the dbl negation
            return true;
        }
    }
    if(principle == "contradiction"){
        if(Expression_type == Expression::ATOMIC){
            if(symbol == '0'){
                Expression_type = Expression::AND;
                operands.push_back(new Expression(*exp_to_add));
                Expression* not_exp = new Expression(*exp_to_add);
                not_exp->negate_in_place(false);
                operands.push_back(not_exp);
                return true;
            }
        }
    }
    if(principle == "tautology"){
        if(Expression_type == Expression::ATOMIC){
            if(symbol == '1'){
                Expression_type = Expression::OR;
                operands.push_back(new Expression(*exp_to_add));
                Expression* not_exp = new Expression(*exp_to_add);
                not_exp->negate_in_place(false);
                operands.push_back(not_exp);
                return true;
            }
        }
    }
    if(principle == "inverse" ){
        if(Expression_type == Expression::NOT){
            if(operands[0]->is_tautology()){
                negate_in_place();
                symbol = '0';
                return true;
            }
            if(operands[0]->is_contradiction()){
                negate_in_place();
                symbol = '1';
                return true;
            }
        }
    }
    if(principle == "identity"){
        if(Expression_type == Expression::OR){
            Expression* new_contradiction = new Expression();
            new_contradiction->Expression_type = Expression::ATOMIC;
            new_contradiction->symbol = '0';
            operands.push_back(new_contradiction);
            return true;
        }
        if(Expression_type == Expression::AND){
            Expression* new_tautology = new Expression();
            new_tautology->Expression_type = Expression::ATOMIC;
            new_tautology->symbol = '1';
            operands.push_back(new_tautology);
            return true;
        }
        if(Expression_type == Expression::ATOMIC){ // arbitrarily pretend it's an AND
            Expression* new_atom = new Expression();
            new_atom->Expression_type = Expression::ATOMIC;
            new_atom->symbol = symbol;
            Expression_type = Expression::AND;
            operands.push_back(new_atom);
            Expression* new_tautology = new Expression();
            new_tautology->Expression_type = Expression::ATOMIC;
            new_tautology->symbol = '1';
            operands.push_back(new_tautology);
            return true;
        }
    }
    if(principle == "annihilation"){
        if(Expression_type == Expression::AND){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_contradiction()){
                    operands.push_back(new Expression(*exp_to_add));
                    return true;
                }
            }
        }
        if(Expression_type == Expression::OR){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_tautology()){
                    operands.push_back(new Expression(*exp_to_add));
                    return true;
                }
            }
        }
        if(Expression_type == Expression::ATOMIC){ // arbitrarily pretend it's an AND
            if(is_tautology()){
                Expression* new_atom = new Expression();
                new_atom->Expression_type = Expression::ATOMIC;
                new_atom->symbol = symbol;
                Expression_type = Expression::OR;
                operands.push_back(new_atom);
                operands.push_back(new Expression(*exp_to_add));
                return true;
            }
            if(is_contradiction()){
                Expression* new_atom = new Expression();
                new_atom->Expression_type = Expression::ATOMIC;
                new_atom->symbol = symbol;
                Expression_type = Expression::AND;
                operands.push_back(new_atom);
                operands.push_back(new Expression(*exp_to_add));
                return true;
            }
        }
    }
    if(principle == "idepotence"){ // attempt to apply the idepotence rule
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){ // this rule applies to ANDs and ORs only
            // find duplicates
            for(int i = 0; i < operands.size(); i++){
                if(*(operands[i]) == *(exp_to_add)){
                    operands.push_back(new Expression(*exp_to_add));
                    return true;
                }
            }
            operands.push_back(new Expression(*operands[0]));
            return true;
        }
        if(Expression_type == Expression::ATOMIC){
            Expression* new_atom = new Expression();
            new_atom->Expression_type = Expression::ATOMIC;
            new_atom->symbol = symbol;
            Expression_type = Expression::AND;
            operands.push_back(new_atom);
            operands.push_back(new_atom);
            return true;
        }
    }
    if(principle=="demorgan"){
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){
            for(int i = 0; i < operands.size(); i++){
                operands[i]->negate_in_place(false);
            }
            reverse_outer_operator();
            negate_in_place(false);
            return true;
        }
    }
    if(principle == "absorption"){ // P^(P|Q) <-> P
        Expression* copy_of_this = new Expression(*this); 
        Expression_type = Expression::AND;
        deleteAllOperands();
        operands.push_back(copy_of_this);
        Expression* new_or = new Expression();
        new_or->Expression_type = Expression::OR;
        new_or->operands.push_back(new Expression(*copy_of_this));
        new_or->operands.push_back(exp_to_add);
        operands.push_back(new_or);
        return true;
    }
    if(principle=="distribution"){ // attempt to map (a^b)|c to (a|c)^(b|c)
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){
            Expression* a_ptr;
            Expression* b_ptr;
            Expression* c1 = new Expression();
            Expression* c2 = new Expression();
            int exp_type = Expression_type;
            vector<Expression*> inner_expressions; // old inner expressions *a* and *b*
            for(int j = 0; j<operands.size();j++){
                if((Expression_type == Expression::AND && operands[j]->Expression_type == Expression::OR)||
                    (Expression_type == Expression::OR && operands[j]->Expression_type == Expression::AND)){
                    // we have a winner
                    a_ptr = operands[j]->operands[0];
                    b_ptr = operands[j];
                    b_ptr->operands.erase(b_ptr->operands.begin()+0);
                    operands.erase(operands.begin()+j);
                    b_ptr->clean();
                    clean();
                    *c1 = *this; // we need to copy c.
                    *c2 = *this;
                    deleteAllOperands();
                    Expression_type = exp_type;
                    reverse_outer_operator();
                    Expression* new_inner_expression1 = new Expression();
                    Expression* new_inner_expression2 = new Expression();
                    new_inner_expression1->Expression_type = exp_type;
                    new_inner_expression2->Expression_type = exp_type;
                    new_inner_expression1->operands.push_back(c1);
                    new_inner_expression1->operands.push_back(a_ptr);
                    new_inner_expression2->operands.push_back(c2);
                    new_inner_expression2->operands.push_back(b_ptr);
                    operands.push_back(new_inner_expression1);
                    operands.push_back(new_inner_expression2);
                    clean();
                    return true;
                }
            }
            delete c1;
            delete c2;
        }
    }
    if(principle=="reduction"){ //  P ∧ (¬P ∨ Q) <-> P ∧ Q
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){ // this rule applies to ANDs and ORs only
            for(int i = 0; i<operands.size();i++){
                if(*operands[i] == *exp_to_add){
                    Expression* exp_q = new Expression(*this);
                    exp_q->remove_operand(i);
                    exp_q->clean();
                    Expression* exp_p = new Expression(*operands[i]);
                    deleteAllOperands();
                    operands.push_back(exp_p);
                    Expression* not_p = new Expression(*exp_p);
                    not_p->negate_in_place(false);
                    Expression* new_or = new Expression();
                    new_or->Expression_type = Expression::OR;
                    new_or->operands.push_back(not_p);
                    new_or->operands.push_back(exp_q);
                    operands.push_back(new_or);
                    return true;
                }
            }
        }
    }
    return false;
}
bool Expression::applyEquivalencePrinciple(string principle){
    if(principle == "double negation"){ // attempt to apply the double negation rule
        if(Expression_type == Expression::NOT && operands[0]->Expression_type == Expression::NOT){
            negate_in_place();
            negate_in_place();
            return true;
        }
    }
    if(principle == "backwards double negation"){
        if(Expression_type != Expression::NOT){
            negate_in_place();
            negate_in_place(false); // does not remove the dbl negation
            return true;
        }
    }
    if(principle == "contradiction"){
        if(Expression_type == Expression::AND){
            for(int j = 0; j < operands.size(); j++){
                Expression opposite_expression;
                opposite_expression.Expression_type = Expression::NOT;
                opposite_expression.operands.push_back(operands[j]);
                for(int i = 0; i < operands.size(); i++){
                    if((i != j) && (*(operands[i]) == opposite_expression)){
                        deleteAllOperands();
                        Expression_type = Expression::ATOMIC;
                        symbol = '0';
                        opposite_expression.operands.clear();
                        return true;
                    }
                }
                opposite_expression.operands.clear();
            }
        }
    }
    if(principle == "tautology"){
        if(Expression_type == Expression::OR){
            for(int j = 0; j < operands.size(); j++){
                Expression opposite_expression;
                opposite_expression.Expression_type = Expression::NOT;
                opposite_expression.operands.push_back(operands[j]);
                for(int i = 0; i < operands.size(); i++){
                    if((i != j) && (*(operands[i]) == opposite_expression)){
                        deleteAllOperands();
                        Expression_type = Expression::ATOMIC;
                        symbol = '1';
                        opposite_expression.operands.clear();
                        return true;
                    }
                }
                opposite_expression.operands.clear();
            }
        }
    }
    if(principle == "inverse" ){
        if(Expression_type == Expression::NOT){
            if(operands[0]->is_tautology()){
                negate_in_place();
                symbol = '0';
                return true;
            }
            if(operands[0]->is_contradiction()){
                negate_in_place();
                symbol = '1';
                return true;
            }
        }
    }
    if(principle == "identity"){
        if(Expression_type == Expression::OR){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_contradiction()){
                    remove_operand(j);
                    return true;
                }
            }
        }
        if(Expression_type == Expression::AND){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_tautology()){
                    remove_operand(j);
                    return true;
                }
            }
        }
    }
    if(principle == "annihilation"){
        if(Expression_type == Expression::AND){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_contradiction()){
                    deleteAllOperands();
                    Expression_type = Expression::ATOMIC;
                    symbol = '0';
                    return true;
                }
            }
        }
        if(Expression_type == Expression::OR){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_tautology()){
                    deleteAllOperands();
                    Expression_type = Expression::ATOMIC;
                    symbol = '1';
                    return true;
                }
            }
        }
    }
    if(principle == "idepotence"){ // attempt to apply the idepotence rule
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){ // this rule applies to ANDs and ORs only
            // find duplicates
            for(int i = 0; i < operands.size(); i++){
                for(int j = i+1; j < operands.size(); j++){
                    if(*(operands[i]) == *(operands[j])){
                        remove_operand(j);
                        return true;
                    }
                }
            }
        }
    }
    if(principle=="demorgan"){
        if(Expression_type == Expression::NOT){
            if(operands[0]->Expression_type == Expression::AND || operands[0]->Expression_type == Expression::OR){
                for(int i = 0; i < operands[0]->operands.size(); i++){
                    operands[0]->operands[i]->negate_in_place(false);
                }
                operands[0]->reverse_outer_operator();
                negate_in_place();
                return true;
            }
        }
    }
    if(principle == "absorption"){ // P^(P|Q) <-> P, only works on atomic P 
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){ // this rule applies to ANDs and ORs only
            for(int j = 0; j<operands.size();j++){
                if((Expression_type == Expression::AND && operands[j]->Expression_type == Expression::OR)||
                    (Expression_type == Expression::OR && operands[j]->Expression_type == Expression::AND)){
                    // if the inner expression type is the opposite of the outer expression type
                    for( int i = 0; i < operands.size(); i++){
                        if((i != j) and (operands[j]->contains(operands[i]))){
                            // if any sub-operand contains one of our outer operands, we will absorb the whole operand
                            remove_operand(j);
                            return true;
                        }
                    }
                }
            }
        }
    }
    if(principle=="distribution_forward"){ // attempt to map (a^b)|c to (a|c)^(b|c)
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){
            Expression* a_ptr;
            Expression* b_ptr;
            Expression* c1 = new Expression();
            Expression* c2 = new Expression();
            int exp_type = Expression_type;
            vector<Expression*> inner_expressions; // old inner expressions *a* and *b*
            for(int j = 0; j<operands.size();j++){
                if((Expression_type == Expression::AND && operands[j]->Expression_type == Expression::OR)||
                    (Expression_type == Expression::OR && operands[j]->Expression_type == Expression::AND)){
                    // we have a winner
                    a_ptr = operands[j]->operands[0];
                    b_ptr = operands[j];
                    b_ptr->operands.erase(b_ptr->operands.begin()+0);
                    operands.erase(operands.begin()+j);
                    b_ptr->clean();
                    clean();
                    *c1 = *this; // we need to copy c.
                    *c2 = *this;
                    deleteAllOperands();
                    Expression_type = exp_type;
                    reverse_outer_operator();
                    Expression* new_inner_expression1 = new Expression();
                    Expression* new_inner_expression2 = new Expression();
                    new_inner_expression1->Expression_type = exp_type;
                    new_inner_expression2->Expression_type = exp_type;
                    new_inner_expression1->operands.push_back(c1);
                    new_inner_expression1->operands.push_back(a_ptr);
                    new_inner_expression2->operands.push_back(c2);
                    new_inner_expression2->operands.push_back(b_ptr);
                    operands.push_back(new_inner_expression1);
                    operands.push_back(new_inner_expression2);
                    clean();
                    return true;
                }
            }
            delete c1;
            delete c2;
        }
    }
    if(principle=="distribution_forward_disjuncts_only"){ // attempt to map (a^b)|c to (a|c)^(b|c)
        if(Expression_type == Expression::OR){
            Expression* a_ptr;
            Expression* b_ptr;
            Expression* c1 = new Expression();
            Expression* c2 = new Expression();
            int exp_type = Expression_type;
            vector<Expression*> inner_expressions; // old inner expressions *a* and *b*
            for(int j = 0; j<operands.size();j++){
                if(operands[j]->Expression_type == Expression::AND){
                    // we have a winner
                    a_ptr = operands[j]->operands[0];
                    b_ptr = operands[j];
                    b_ptr->operands.erase(b_ptr->operands.begin()+0);
                    operands.erase(operands.begin()+j);
                    b_ptr->clean();
                    clean();
                    *c1 = *this; // we need to copy c.
                    *c2 = *this;
                    deleteAllOperands();
                    Expression_type = exp_type;
                    reverse_outer_operator();
                    Expression* new_inner_expression1 = new Expression();
                    Expression* new_inner_expression2 = new Expression();
                    new_inner_expression1->Expression_type = exp_type;
                    new_inner_expression2->Expression_type = exp_type;
                    new_inner_expression1->operands.push_back(c1);
                    new_inner_expression1->operands.push_back(a_ptr);
                    new_inner_expression2->operands.push_back(c2);
                    new_inner_expression2->operands.push_back(b_ptr);
                    operands.push_back(new_inner_expression1);
                    operands.push_back(new_inner_expression2);
                    clean();
                    return true;
                }
            }
            delete c1;
            delete c2;
        }
    }
    if(principle=="distribution_forward_conjuncts_only"){ // attempt to map (a^b)|c to (a|c)^(b|c)
        if(Expression_type == Expression::AND){
            Expression* a_ptr;
            Expression* b_ptr;
            Expression* c1 = new Expression();
            Expression* c2 = new Expression();
            int exp_type = Expression_type;
            vector<Expression*> inner_expressions; // old inner expressions *a* and *b*
            for(int j = 0; j<operands.size();j++){
                if(operands[j]->Expression_type == Expression::OR){
                    // we have a winner
                    a_ptr = operands[j]->operands[0];
                    b_ptr = operands[j];
                    b_ptr->operands.erase(b_ptr->operands.begin()+0);
                    operands.erase(operands.begin()+j);
                    b_ptr->clean();
                    clean();
                    *c1 = *this; // we need to copy c.
                    *c2 = *this;
                    deleteAllOperands();
                    Expression_type = exp_type;
                    reverse_outer_operator();
                    Expression* new_inner_expression1 = new Expression();
                    Expression* new_inner_expression2 = new Expression();
                    new_inner_expression1->Expression_type = exp_type;
                    new_inner_expression2->Expression_type = exp_type;
                    new_inner_expression1->operands.push_back(c1);
                    new_inner_expression1->operands.push_back(a_ptr);
                    new_inner_expression2->operands.push_back(c2);
                    new_inner_expression2->operands.push_back(b_ptr);
                    operands.push_back(new_inner_expression1);
                    operands.push_back(new_inner_expression2);
                    clean();
                    return true;
                }
            }
            delete c1;
            delete c2;
        }
    }
    if(principle=="distribution_backwards"){ // attempt to map (a|c)^(b|c) to (a^b)|c
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){ // this rule applies to ANDs and ORs only
            for(int i = 0; i < operands.size(); i++){
                for( int j = i+1; j < operands.size(); j++){
                    for(int k = 0; k < operands[i]->operands.size(); k++){
                        for(int l = 0; l < operands[j]->operands.size(); l++ ){
                            if(*(operands[i]->operands[k]) == *(operands[j]->operands[l])){
                                Expression* a_ptr = operands[i];
                                Expression* b_ptr = operands[j];
                                Expression* c_ptr = operands[i]->operands[k];
                                a_ptr->operands.erase(a_ptr->operands.begin()+k); // don't delete the child here, it is pointed to by c
                                b_ptr->remove_operand(l); // erase this child, it is an orphan
                                Expression* ab_ptr = new Expression();
                                ab_ptr->Expression_type = Expression_type;
                                ab_ptr->operands.push_back(a_ptr);
                                ab_ptr->operands.push_back(b_ptr);
                                operands.erase(operands.begin()+i);
                                operands.erase(operands.begin()+j-1); // the -1 is because i<j was just removed
                                Expression* new_inner_expression = new Expression();
                                new_inner_expression->Expression_type = Expression_type;
                                new_inner_expression->reverse_outer_operator();
                                new_inner_expression->operands.push_back(c_ptr);
                                new_inner_expression->operands.push_back(ab_ptr);
                                operands.push_back(new_inner_expression);
                                clean();
                                return true;
                            }
                        }
                    }
                }
            }
            
        }
    }

    if(principle=="reduction"){ //  P ∧ (¬P ∨ Q) <-> P ∧ Q
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){ // this rule applies to ANDs and ORs only
            for(int j = 0; j<operands.size();j++){
                if((Expression_type == Expression::AND && operands[j]->Expression_type == Expression::OR)||
                    (Expression_type == Expression::OR && operands[j]->Expression_type == Expression::AND)){
                    // if the inner expression type is the opposite of the outer expression type
                    for(int k = 0; k < operands[j]->operands.size(); k++){
                        for( int i = 0; i < operands.size(); i++){
                            operands[i]->negate_in_place();
                            if(*(operands[i])==*(operands[j]->operands[k])){
                                // if any sub-operand matches one of our outer operands, we will absorb the whole operand
                                operands[j]->operands[k]->negate_in_place();
                                operands[j]->break_generalization(k);
                                operands[j]->reverse_outer_operator();
                                remove_operand(i);
                                return true;
                            }
                            operands[i]->negate_in_place(); 
                            // this will actually apply dbl negation, 
                            // so we should make sure to not run this on expressions with dbl negations or we may skip a step
                        }
                    }
                }
            }
        }
    }
    return false;
}
Expression* Expression::get_random_expression(){
    if(Expression_type == Expression::ATOMIC){
        return this;
    }
    if(Expression_type == Expression::NOT){
        int random = rand() % 2;
        cout << random << endl;
        if(random == 0){
            return this;
        }
        else{
            return operands[0]->get_random_expression();
        }
    }
    int random = rand() % operands.size()+1;
    cout << random << endl;
    if(random == operands.size()){
        return this;
    }
    else{
        return operands[random]->get_random_expression();
    }
}
Expression* Expression::get_random_operand(){
    if(Expression_type == Expression::ATOMIC){
        return this;
    }
    int random = rand() % operands.size();
    return operands[random];
}
bool Expression::contains(Expression* other_expression){
    if(other_expression->Expression_type == Expression::ATOMIC ||
     other_expression->Expression_type == Expression::NOT){
        for(int i = 0; i < operands.size(); i++){
            if(*(operands[i])==*other_expression){
                return true;
            }
        }
        return false;  
    }
    if(Expression_type != other_expression->Expression_type){
        return false;
    }
    vector<Expression*> unused_other_ops = other_expression->operands;
    for(int i = 0; i < operands.size(); i++){
        for(int j = 0; j < unused_other_ops.size(); j++){
            if(*(operands[i]) == *(unused_other_ops[j])){
                unused_other_ops.erase(unused_other_ops.begin()+j);
                j--;
            }
        }
    }
    return (unused_other_ops.size() == 0);
}
void Expression::remove_operand(int i){
    delete operands[i];
    operands.erase(operands.begin()+i);
}
void Expression::break_generalization(int index){
    Expression* temp_ptr = operands[index];
    operands.erase(operands.begin()+index);
    Expression* new_outer_expression = new Expression();
    new_outer_expression->operands = operands;
    new_outer_expression->Expression_type = Expression_type;
    operands.clear();
    operands.push_back(temp_ptr);
    operands.push_back(new_outer_expression);
    temp_ptr = NULL;
}
// O(1) in-place negation, woohoo
void Expression::negate_in_place(bool auto_simplify){
    if(Expression_type == Expression::NOT && (auto_simplify)){
        Expression* temp_ptr = operands[0];
        operands = temp_ptr->operands;
        symbol = temp_ptr->symbol;
        Expression_type = temp_ptr->Expression_type;
        temp_ptr->operands.clear();
        delete temp_ptr;
    }
    else{
        Expression* new_inner_expression = new Expression();
        new_inner_expression->Expression_type = Expression_type;
        new_inner_expression->symbol = symbol;
        new_inner_expression->operands = operands;
        Expression_type = Expression::NOT;
        operands.clear();
        operands.push_back(new_inner_expression);
    }
}
void Expression::reverse_outer_operator(){
    if(Expression_type == Expression::AND){
        Expression_type = Expression::OR;
    }
    else{
        if(Expression_type == Expression::OR){
            Expression_type = Expression::AND;
        }
        else{
            cerr << "Error: reverse_outer_operator called on incorrect expression type" << endl; 
        }
    }
}
// O(1) conjunction -- in-place
void Expression::conjunct_in_place_with(Expression* expr){
    Expression* new_inner_expression = new Expression();
    new_inner_expression->Expression_type = Expression_type;
    new_inner_expression->symbol = symbol;
    new_inner_expression->operands = operands;
    Expression_type = Expression::AND;
    operands.clear();
    operands.push_back(new_inner_expression);
    operands.push_back(expr);
}
// O(1) disjunction -- in-place
void Expression::disjunct_in_place_with(Expression* expr){
    Expression* new_inner_expression = new Expression();
    new_inner_expression->Expression_type = Expression_type;
    new_inner_expression->symbol = symbol;
    new_inner_expression->operands = operands;
    Expression_type = Expression::OR;
    operands.clear();
    operands.push_back(new_inner_expression);
    operands.push_back(expr);
}
void Expression::clean(){ // merges ANDs and ORs for commutivity and removes ANDs and ORs with 1 operand
    if(Expression_type == Expression::ATOMIC){
        return;
    }
    for(int i = 0; i < operands.size(); i++){
        operands[i]->clean();
    }
    vector<Expression*> temporary_operand_list;
    if(Expression_type == Expression::AND || Expression_type == Expression::OR){
        for(int i = 0; i < operands.size(); i++){
            if(operands[i]->Expression_type == Expression_type){
                temporary_operand_list.insert(temporary_operand_list.end(),
                    operands[i]->operands.begin(), operands[i]->operands.end());
                operands[i]->operands.clear();
                delete operands[i];
                operands.erase(operands.begin()+i);
                i--;
            }
        }
    }
    for(int i = 0; i < temporary_operand_list.size(); i++){
        operands.push_back(temporary_operand_list[i]);
    }
    if(Expression_type == Expression::AND || Expression_type == Expression::OR){
        if(operands.size()==1){
            Expression* temp_ptr = operands[0];
            operands = temp_ptr->operands;
            symbol = temp_ptr->symbol;
            Expression_type = temp_ptr->Expression_type;
            temp_ptr->operands.clear();
            delete temp_ptr;
        }
    }
}

/* String conversion functions */

string Expression::getMathematically() const{
    string retStr = "";
    if(!is_atomic_symbol()){
        if(Expression_type == Expression::NOT){
            retStr += "NOT(";
        }
        if(Expression_type == Expression::AND){
            retStr += "AND(";
        }
        if(Expression_type == Expression::OR){
            retStr += "OR(";
        }
        retStr += operands[0]->getMathematically();
        for(int i = 1; i < operands.size(); i++){
            retStr += "," + operands[i]->getMathematically();
        }
        retStr += ")";
    }
    else{
        retStr += symbol;
    }
    return retStr;
}
void Expression::printMathematically() const{
    cout << getMathematically() << endl;
}
string Expression::getHumanReadable(bool is_inner, bool exact_symbols) const{
    string retStr = "";
    if(Expression_type == Expression::NOT){
        retStr += "~";
        retStr += operands[0]->getHumanReadable(true, exact_symbols);
    }
    if(Expression_type == Expression::AND){
        retStr += "(";
        retStr += operands[0]->getHumanReadable(true, exact_symbols);
        for(int i = 1; i<operands.size();i++){
            retStr += "^" + operands[i]->getHumanReadable(true, exact_symbols);
        }
        retStr += ")";
    }
    if(Expression_type == Expression::OR){
        retStr += "(";
        retStr += operands[0]->getHumanReadable(true, exact_symbols);
        for(int i = 1; i < operands.size(); i++){
            retStr += "|" + operands[i]->getHumanReadable(true, exact_symbols);
        }
        retStr += ")";
    }
    if(Expression_type == Expression::ATOMIC){
        if(is_contradiction() && !exact_symbols){
            retStr+= "\u22A5 ";
        }
        else if(is_tautology() && !exact_symbols){
            retStr+= "\u22A4 ";
        }
        else{
            retStr += symbol;
        }
    }
    if(!is_inner){
        if(retStr.substr(0,1) == "(" and retStr.substr(retStr.length()-1,1) == ")"){
            retStr = retStr.substr(1,retStr.length()-2);
        }
    }
    return retStr;
}
void Expression::printHumanReadable() const{
    string output = getHumanReadable();
    cout << output << endl;
}
string Expression::getLatex() const{
    string retStr = "";
    if(Expression_type == Expression::NOT){
        retStr += "\\lnot (";
        retStr += operands[0]->getLatex();
        retStr += ")";
    }
    if(Expression_type == Expression::AND){
        retStr += "(";
        retStr += operands[0]->getLatex();
        for(int i = 1; i<operands.size();i++){
            retStr += "\\wedge " + operands[i]->getLatex();
        }
        retStr += ")";
    }
    if(Expression_type == Expression::OR){
        retStr += "(";
        retStr += operands[0]->getLatex();
        for(int i = 1; i < operands.size(); i++){
            retStr += "\\vee " + operands[i]->getLatex();
        }
        retStr += ")";
    }
    if(Expression_type == Expression::ATOMIC){
        if(is_contradiction()){
            retStr+= "\\bot";
        }
        else if(is_tautology()){
            retStr+= "\\top";
        }
        else{
            retStr += symbol;
        }
    }
    return retStr;
}
void Expression::printLatex() const{
    cout <<getLatex() << "\\\\" << endl;
}
void printTransformation(const Expression& from , const Expression& to , const string method){
    if(from.suppress_output){
        return;
    }
    string method_cpy = method;
    std::replace(method_cpy.begin(), method_cpy.end(), '_', ' ');
    cout << "By "<< method_cpy << " becomes"  << endl;

}
#else

#endif
