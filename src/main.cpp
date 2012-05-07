#include <iostream>
#include <fstream>
#include "Parser.h"
#include "processors/PrettyPrinter.h"


using std::string;
using std::ifstream;

using MrFusion::AsmParser;
using MrFusion::Processors::PrettyPrinter;

int main(int argc, char **argv) {
    string str;
    
    AsmParser parser;
    
    string sample = 
    "; Try some basic stuff\n"
    "                  SET A, 0x30              ; 7c01 0030\n"
    "                  SET [0x1000], 0x20       ; 7de1 1000 0020\n"
    "                  SUB A, [0x1000]          ; 7803 1000\n"
    "                  IFN A, 0x10              ; c00d \n"
    "                     SET PC, crash         ; 7dc1 001a [*]\n"
    "                  \n"
    "    ; Do a loopy thing\n"
    "                  SET I, 10                ; a861\n"
    "                  SET A, 0x2000            ; 7c01 2000\n"
    "    :loop         SET [0x2000+I], [A]      ; 2161 2000\n"
    "                  SUB I, 1                 ; 8463\n"
    "                  IFN I, 0                 ; 806d\n"
    "                     SET PC, loop          ; 7dc1 000d [*]\n"
    "    \n"
    "    ; Call a subroutine\n"
    "                  SET X, 0x4               ; 9031\n"
    "                  JSR testsub              ; 7c10 0018 [*]\n"
    "                  SET PC, crash            ; 7dc1 001a [*]\n"
    "    \n"
    "    :testsub      SHL X, 4                 ; 9037\n"
    "                  SET PC, POP              ; 61c1\n"
    "                    \n"
    "    ; Hang forever. X should now be 0x40 if everything went right.\n"
    "    :crash        SET PC, crash            ; 7dc1 001a [*]\n"
    "    \n"
    "    ; [*]: Note that these can be one word shorter and one cycle faster by using the short form (0x00-0x1f) of literals,\n"
    "    ;      but my assembler doesn't support short form labels yet.\n";
    
    
    parser.parseIt(sample, "somefile.asm");
    PrettyPrinter pp(std::cout);
    parser.context()->processLines(pp);
    
    std::cout << "Bye... :-) \n\n";
    return 0;
}
