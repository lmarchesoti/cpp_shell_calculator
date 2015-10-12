#include <vector>
#include <iostream>
#include <boost/xpressive/xpressive.hpp>
#include <map>
using namespace std;
using namespace boost;
using namespace xpressive;

class Calculator{
private:
	/* attributes */
	//keeps track of operation mode
	char precision;
	//error register
	string error;
	//memory registers
	vector<string> memory;
	
	/* methods */
	//decides what to do
	void parseAction(string);
	
	//evaluates expression with template precision
	template<typename T>
	void evalExpr(vector<string>);
		
	//parses the expression before evaluating
	vector<string> parseExpr(string);
	
	//implements a simplified shunting yard algorithm
	//for parsing the expression into a string vector
	vector<string> simplifiedShuntingYard(string);
	
	//sets memory position
	void memset(int);
	
	//clears memory
	void memclear(int);
		
	//pretty print used every round
	void print(void);
	
public:
	Calculator(char);
	~Calculator(void);
	
	//main loop
	void run(void);
	
};
