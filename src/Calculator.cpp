#include <cmath>
#include "Calculator.h"
#include <iostream>
#include <stack>
#include <boost/xpressive/xpressive.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace boost;
using namespace xpressive;

Calculator::Calculator(char type){
	//sets default precision
	precision = type;
	//makes starting last calculation value = 0
	memory.push_back("0");
	//no error message;
	error = "";
}

Calculator::~Calculator(void){
}

//main function of calculator
void Calculator::run(void){
	string comm;
	//main loop
	while(true){
		//starts by printing
		print();
		//take input expression
		getline(cin, comm);
		//parses input and acts acoordingly
		parseAction(comm);
	}
}

//decides and executes actions accordingly
void Calculator::parseAction(string comm){
	sregex rx;
    smatch result;
    vector<string> expression;
    //clear or clear M#
	if(regex_match(comm, result, rx = ("clear" >> !( " M" >> (s1 = +_d))))){
		//if clear M
		if(result[s1].str() != "")
			//clears position stated
			try {
				memclear(lexical_cast<int>(result[s1].str()));
			} catch(bad_lexical_cast &) {
				//throws error if cast fails for some weird reason
				error = "Bad memory position";
			}
		else
			//clear everything
			memclear(-1);
		return;
	}
	//set memory command - [M#]
	if(regex_match(comm, result, rx = "[M" >> (s1 = +_d) >> "]")){
		//sets memory at stated position
		try {
			memset(lexical_cast<int>(result[s1].str()));
		} catch(bad_lexical_cast &) {
			//throws error if cast fails for some weird reason
			error = "Bad memory position";
		}
		return;
	}
	//changes precision
	if(regex_match(comm, result, rx = "-" >> (s1 = as_xpr("i") | "f" | "d"))){
		precision = result[s1].str().at(0);
		return;
	}
	//exit
	if(regex_match(comm, result, rx = as_xpr("exit"))){
		exit(0);
	}
	//defaults to expression evaluation
	expression = parseExpr(comm);
	//if theres an error, abort
	if(error != "")
		return;
	//if not, evaluate expression
	//according to precision
	switch(precision){
		//integer
		case 'i':
			evalExpr<int>(expression);
			break;
		//float
		case 'f':
			evalExpr<float>(expression);
			break;
		//double
		case 'd':
			evalExpr<double>(expression);
			break;
		//This shouldn't happen..
		default:
			error = "Precision Error";
	}		
}

//evaluate expression in the vector
//using precision T
template <typename T>
void Calculator::evalExpr(vector<string> expression){
	//stack to store operands
	stack<T> operandStack;
	//temp variables for operations
	T t1, t2;
	for(int i = 0; i < expression.size(); ++i){
		cout << expression.at(i) << endl;
		//action based on the position of the expression
		switch(expression.at(i).at(0)){
			//multiplication
			case '*':
				//if theres less than 2 operands in the stack, something's missing
				if(operandStack.size() < 2){
					error = "Missing Operand";
					return;
				}
				//pop operands
				t1 = operandStack.top();
				operandStack.pop();
				t2 = operandStack.top();
				operandStack.pop();
				//and evaluate proper expression
				//pushing the value onto the operand stack
				operandStack.push(t2 * t1);
				break;
			//division
			case '/':
				if(operandStack.size() < 2){
					error = "Missing Operand";
					return;
				}
				t1 = operandStack.top();
				operandStack.pop();
				t2 = operandStack.top();
				operandStack.pop();
				operandStack.push(t2 / t1);
				break;
			//addition
			case '+':
				if(operandStack.size() < 2){
					error = "Missing Operand";
					return;
				}
				t1 = operandStack.top();
				operandStack.pop();
				t2 = operandStack.top();
				operandStack.pop();
				operandStack.push(t2 + t1);
				break;
			//subtraction
			case '-':
				if(operandStack.size() < 2){
					error = "Missing Operand";
					return;
				}
				t1 = operandStack.top();
				operandStack.pop();
				t2 = operandStack.top();
				operandStack.pop();
				operandStack.push(t2 - t1);
				break;
			//operand
			default:
				//if it's an int, needs special conversion
				//or it crashes because of the dot
				if(typeid(T).name() == typeid(int).name()){
					//truncates
					//expression.at(i) = regex_replace(expression.at(i), sregex::compile("\\.\\d+"), "");
					//rounds
					expression.at(i) = lexical_cast<string>(floor(lexical_cast<double>(expression.at(i))+0.5));
				}
				//and just push it onto the stack
				operandStack.push(lexical_cast<T>(expression.at(i)));
		}
	}
	//if theres more than the result on the stack
	//something's missing
	if(operandStack.size() > 1)
		error = "Missing Operator";
	//make last operation = result
	else{
		//stores unsigned values only
		//if not, it bugs the parser
		//the end result of a negative value can still be achieved anyways
		if(operandStack.top() < 0){
			error = "Storing unsigned value\n";
			//still shows the actual result
			error += "Result: " + lexical_cast<string>(operandStack.top());
			memory.at(0) = lexical_cast<string>(abs(operandStack.top()));
		}
		else
			memory.at(0) = lexical_cast<string>(operandStack.top());
	}
}

//parses the expression into a vector
//gives error messages accordingly
vector<string> Calculator::parseExpr(string expr){
	sregex rx;
	smatch match;
	vector<string> expression;
	//trimming spaces from the expression
	//without validating, as a typo aid for "mul t" and such
	//using regex since it's already everywhere
	expr = regex_replace(expr, rx = +_s, "");
	//reducing the cases I have to work with
	//regex_replace with formatter objects turned out to be much more than needed for this
	//so I'm using the simple less-efficient method
	expr = regex_replace(expr, rx = as_xpr("add"), "+");
	expr = regex_replace(expr, rx = as_xpr("sub"), "-");
	expr = regex_replace(expr, rx = as_xpr("mul"), "*");
	expr = regex_replace(expr, rx = as_xpr("div"), "/");
	//expressions starting with operators default to M0 expr
	if(regex_match(expr, rx = (as_xpr("+") | "-" | "*" | "/") >> +_))
		expr = memory.at(0) + expr;
	//substitutes [M#] for the actual values
	while(regex_search(expr, match, rx = ("[M" >> (s1=+_d) >> "]")))
		//checks if position is valid
		if(memory.size() > lexical_cast<int>(match[s1].str())){
			expr = regex_replace(expr, rx = ("[M" >> as_xpr(match[s1].str()) >> "]"), memory.at(lexical_cast<int>(match[s1].str())));
		}
		else{
			//if not, raise an error and stop parsing
			error = "Bad memory position: " + match[0].str();
			return expression;
		}
	//converts the expression to postfix using a simplified shunting yard algorithm
	return simplifiedShuntingYard(expr);
}

//implements a simplified shunting yard algorithm that parses the expression into a vector
//simplified because it doesn't have all the operators nor functions
vector<string> Calculator::simplifiedShuntingYard(string expr){
	vector<string> exprStack;
	stack<string> operatorStack;
	
	//separate into tokens
    sregex token = (+_d >> !("." >> +_d))					//either operand
   					| "*" | "/" | "+" | "-" | "(" | ")";	//or operator

    sregex_iterator cur( expr.begin(), expr.end(), token );
    sregex_iterator end;

	//for each token
    for( ; cur != end; ++cur )
    {
        smatch const &what = *cur;
        //takes action based on token
        switch(what[0].str().at(0)){
        	//if operator *
        	case '*':
        		//while operator on top of the stack has equal priority
        		while(!operatorStack.empty() &&
        			(operatorStack.top() == "*" || operatorStack.top() == "/")){
        				//pop onto results
        				exprStack.push_back(operatorStack.top());
        				operatorStack.pop();
        		}
        		//then push operator onto the stack
        		operatorStack.push("*");
        		break;
        	//if operator /, same from *
        	case '/':
        		while(!operatorStack.empty() &&
        			(operatorStack.top() == "*" || operatorStack.top() == "/")){
        				exprStack.push_back(operatorStack.top());
        				operatorStack.pop();
        		}
        		operatorStack.push("/");        	
        		break;
        	//if operator +
        	case '+':
        		//while theres a higher priority operator on top of the stack
        		while(!operatorStack.empty() &&
        			(operatorStack.top() == "*" || operatorStack.top() == "/")){
        				//pop it onto results
        				exprStack.push_back(operatorStack.top());
        				operatorStack.pop();
        		}
        		//and only then push the operator into the stack
        		operatorStack.push("+");
        		break;
        	//if operator -, same from +
        	case '-':
        		while(!operatorStack.empty() &&
        			(operatorStack.top() == "*" || operatorStack.top() == "/")){
        				exprStack.push_back(operatorStack.top());
        				operatorStack.pop();
        		}
        		operatorStack.push("-");
        		break;
        	//if its a left parenthesis
        	case '(':
        		//push onto the stack
        		operatorStack.push("(");
        		break;
        	//if its a right parenthesis
        	case ')':
        		//until theres a left parenthesis on top of the stack
        		while(!operatorStack.empty() &&
	        		operatorStack.top() != "("){
	        			//pop operators onto results
        				exprStack.push_back(operatorStack.top());
        				operatorStack.pop();
        		}
        		//if the stack becomes empty before finding a parenthesis
        		if(operatorStack.empty()){
        			//the expression is unbalanced
        			error = "unbalanced parentheses";
					return exprStack;
        		}
        		//if not, discard both parentheses
        		else
        			operatorStack.pop();
        		break;
        	//if theres an operand
        	default:
        		//no need to validate again, it only matches valid tokens at this point
        		//push it into the final stack
        		exprStack.push_back(what[0].str());
        }
    }
    //while there are still operators left
    while(!operatorStack.empty())
    	//if theres still a parenthesis in there
    	if(operatorStack.top() == "("){
    		//the expression is unbalanced
    		error = "unbalanced parentheses";
			return exprStack;
    	}
    	//if it's alright
    	//keep popping operators onto results
    	else{
			exprStack.push_back(operatorStack.top());
			operatorStack.pop();
    	}
	//if the stack is empty, the parser didn't recognize any tokens
	//it was probably a wrong command
	//could be caught before the expression parsing by comparing cur == end
    if(exprStack.empty())
    	error = "invalid command";

    //return the stack with postfix operation
	return exprStack;
}

void Calculator::memset(int pos){
	//if requested position higher than what's already there
	if(pos > memory.size()-1)
		//insert at the end for not overcomplicating simple things
		memory.push_back(lexical_cast<string>(memory.at(0)));
	else
		//inserts last calculation at asked position
		memory.at(pos) = memory.at(0);
}

//clears position of or entire memory
void Calculator::memclear(int pos){
	//clear based on position argument
	switch(pos){
		case -1:
			//clear everything
			memory.clear();
			//and make last calculation = 0
			memory.push_back("0");
			break;
		//clearing last operation
		case 0:
			//just set it to 0
			memory.at(0) = "0";
			break;
		//everything else
		default:
			//check if the position is valid
			if(memory.size() > pos)
				//if so, clear position
				memory.erase(memory.begin() + pos);
			else
				//if not, error
				error = "Nothing to clear";	
	}
}

//just prints
void Calculator::print(void){
	//clear screen
	write(1,"\E[H\E[2J",7);
	//prints memory
	for(int i = memory.size() - 1; i > 0; --i)
		cout << "M" << i << ": " << memory.at(i) << endl;
	//prints last calculation
	cout << "Last calculation: " << memory.at(0) << endl;
	//prints error if there's any
	if(error.length() != 0){
		cout << "! " << error << endl;
		//clears error register so it won't repeat
		error = "";
	}
	//cursor indicating input
	cout << ">";	
}
