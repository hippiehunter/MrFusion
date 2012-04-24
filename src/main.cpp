#include <iostream>
#include <fstream>
#include "DCPU16Parser.h"


using std::string;
using std::ifstream;

int main(int argc, char **argv) {
    string str;
    
    AsmParser parser;
    
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

	ifstream file;
	file.open(str);
        
	string data << file;
	parser.parseIt(data, str);
    }
    
    std::cout << "Bye... :-) \n\n";
    return 0;
}
