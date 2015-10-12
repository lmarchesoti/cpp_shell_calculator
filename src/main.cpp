#include "Calculator.h"
#include <boost/xpressive/xpressive.hpp>
#include <iostream>
using namespace std;
using namespace boost;
using namespace xpressive;

int main(int argc, char *argv[]){
	//regex for matching parameters
	sregex rx = "-" >> (s1 = as_xpr("i") | "f" | "d");
    smatch mode;
    //defaults to float
    char type = 'f';
	if(argc - 1){
		string param = argv[1];
		//won't take an invalid parameter
		//if that's the case, still defaults to float
		if(regex_match(param, mode, rx))
			type = mode[s1].str().at(0);
	}
	//create a calculator with the required precision
	Calculator* calcMngr = new Calculator(type);
	//starts functioning
	calcMngr->run();
}
