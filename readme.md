# PLUS -- Propositional Logic Utility Suite
## Authors
2013:
Dan Bulger

## About
Features:

Direct manipulation of tree representation of an expression:
	User can type an expression and convert it into a tree that represents the expression.
	The user is then able to directly manipulate the expression by adding or removing ANDs, ORs, NOTs, and atomic variables.
	When the user is done editing the expression he may turn it back into a text string.  Be careful when doing this, 
	if the tree does not represent a syntactically correct expression, the program may crash.
	
Manual simplification or expansion of an expression:
	After entering an expression into the tree panel, the user is able to use the equivalence rules discussed in class to either simplify or expand
	statements.  The program will at each step indicate whether the expression displayed in the tree panel is in 
	CNF, DNF, and NNF.  If desired, the user may also generate source for a LaTeX document that documents the simplification of an expression by clicking the "Record Simplification" button.
	Once the button is clicked, the user is confined to only using the equivalence rules supplied to simplify the expression.  Once he is done simplifying,
	he should click the "Record" button again.  This will save the LaTeX source, and the user may use any Tex document generator to compile the source into a clean
	proof of simplification to show off to his friends.

Automatic simplification of an expression:
	If the user does not wish to simplify an expression himself, he may elect to have PLUS simplify an expression for him.
	The simplifier will put any expression into CNF or DNF, but DOES NOT always produce the "simplest" form.
	I have found, however, that repeatedly simplifying to CNF then DNF etc... eventually results in the simplest expression.

Automatic expansion of an expression:
	The final function of PLUS is to help generate "random" expansions of simple propositional logic expressions.
	After entering an initial expression, the user may select which expansion rules he wishes to have applied to the statement, and how many times the expression should be expanded.
	PLUS will attempt to apply a random selected rule to a random portion of the expression, and will repeat this process until it has successfully expanded the expression the specified number of times.
	This often leads to many applications of rules that can be applied anywhere, so if the user wishes to have more control over the generation, he may expand an expression using only a few rules first,
	then copy the resulting expression back into the generator and apply a new set of expansion rules to it.
	
	
	
Syntax:
	PLUS only works with AND, OR, NOT, and atomic variables when simplifying, displaying, and expanding proofs, but the parser will recognize implication and biconditional statements as well.
	The parser will merely transform these statements into their equivalents using ands, ors, and nots.
	
	The parser that turns a text string into an expression object recognizes the following:
	'|' and '+' represent an OR clause
	'^','&', and '*' represent an AND clause
	'~','!', and '-' represent a NOT clause
	'->' represents an implication
	'<->' represents a biconditional
	Any alphanumeric character is recognized as an atomic variable(e.g. [A-z,0-9])
	The character '0' is recognized specially as a contradiction
	The character '1' is recognized specially as a tautology
