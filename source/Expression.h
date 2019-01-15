#ifndef EXPRESSIONH
#define EXPRESSIONH
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
#include <iostream>
#include <cstdlib>

using namespace std;
struct ExpressionComparator;
class Expression{
public:

    /* class variables */

    static vector<string> simplification_rules;
    static bool suppress_output; // controls whether the simplify() function writes to cout
    vector<Expression*> operands; // will hold the Expression's operand(s)
    char symbol; // this will be set if this Expression object represents an atomic statement
    enum Expression_types{
        OR, AND, NOT, IMPLICATION, BICONDITIONAL, ATOMIC
    };
    int Expression_type;

    /* Functions */

    Expression();
    // Copy constructor
    Expression(const Expression& existingExpression);
    Expression& operator= (const Expression& existingExpression);
    ~Expression();
    bool is_atomic_symbol() const;
    void clean(); // merges connected ANDs and ORs for associativity efficiency -- a^(b^c) -> a^b^c
    void expand_to_cnf();
    void expand_to_dnf();
    bool is_in_cnf();
    bool is_in_dnf();
    bool is_in_nnf();
    void simplifyCompletely(bool dnf = false);
    bool simplify(vector<Expression*>& expressions_expanded, bool& expanded, bool dnf = false); // attempts to simplify the function.  Returns true if function has changed
    void remove_operand(int i);
    void deleteAllOperands();
    void break_generalization(int index);
    bool contains(Expression* other_expression);
    bool applyInferenceRule(string str); // attempts to apply the nth defined inference rule.  Returns true if inference rule is applied
    bool applyEquivalencePrinciple(string principle);
    bool expandExpression(string principle, Expression* exp_to_add = NULL);
    bool reorder(int i);
    void reorder_outer_operands();
    void reverse_outer_operator();
    void negate_in_place(bool auto_simplify = true);
    void conjunct_in_place_with(Expression* expr);
    void disjunct_in_place_with(Expression* expr);
    Expression* get_random_expression();
    Expression* get_random_operand();
    pair<int,int> getOperandsWithMatches() const;
    string getHumanReadable(bool is_inner = false, bool exact_symbols = false) const;
    void printHumanReadable() const;
    string getLatex() const;
    void printLatex() const;
    string getMathematically() const;
    void printMathematically() const;
    bool is_tautology() const{ return(Expression_type == Expression::ATOMIC && symbol == '1'); }
    bool is_contradiction() const{ return(Expression_type == Expression::ATOMIC && symbol == '0'); }
    bool is_atom_or_negation_of_atom();
    bool operator<(const Expression& other) const;
    bool operator>(const Expression& other){return !(*(this) < other);}
};
/*struct ExpressionComparator {
    inline bool operator()(const Expression* expr1, const Expression* expr2) const {
        return (*(expr1) < *(expr2));
    }
};*/

bool operator==(const Expression& lhs, const Expression& rhs);
bool equals(const Expression& lhs, const Expression& rhs);
bool operator!=(const Expression& lhs, const Expression& rhs);
#else

#endif
