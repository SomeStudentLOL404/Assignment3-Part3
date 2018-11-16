#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include "tokens.h"
#include "parse.h"
#include "parsetree.h"
#include "tokens.h"

// #include <cxxabi.h>

using namespace std;

extern map<string, int> mapv;

//Credits to Nick Delello
/*
array<string, 4> names = {"ERRTYPE", "INTTYPE", "STRTYPE", "BOOLTYPE"};

//Demangles class names when printing them out for debugging.
string demangle(const char* name) {
    int status;
    char* demangledName = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    string retVal = string(demangledName);
    free(demangledName);
    return retVal;
}

// Prints out the parse tree if debug is enabled.
void debugPrint(ParseTree* tree, uint level = 0) {
    ParseTree* currTree = tree->getLeft();
    string className = demangle(typeid(*tree).name());
    cout << string(level, '\t') << className << string(8 - level - className.length() / 4, '\t') << names[tree->GetType()] << '\n';
    if (currTree)
        debugPrint(currTree, level + 1);
    currTree = tree->getRight();
    if (currTree)
        debugPrint(currTree, level + (className != "StmtList"));
}
*/

int main(int argc, char* argv[])
{
    ifstream infile1;
    istream *in = &cin;
    int linenum = 0;
    for(int i = 1; i < argc; i++)
    {
        string arg(argv[i]);
        if(in != &cin)
        {
            cout << "TOO MANY FILENAMES" << endl;
            return -1;
        }
        else {
            infile1.open(arg);
            if (infile1.is_open() == false)
            {
                cout << "COULD NOT OPEN " << arg << endl;
                return -1;
            }
            in = &infile1;
        }
    }
    ParseTree *prog = Prog(in, &linenum);
    if( prog == 0 )
    {
        return 0; // quit on error
    }
    // debugPrint(prog);


    cout << "LEAF COUNT: " << prog->LeafCount() << endl;
    cout << "STRING COUNT: " << prog->StringCount() << endl;

    if(prog->IdentCount() != 0)
    {
        cout << "IDENT COUNT: " << prog->IdentCount() << endl;
        int max = 0;
        string out = "";
        /*
        First For Loop - Gets the Maximum
        map<string, int>
           <key=string=IDENT, amount of times seen>

        In Each Loop, we check to see if its the "max"

        max
        */
        for (auto it = mapv.begin(); it != mapv.end(); it++)
        {
            if (it->second > max)
            {
                max = it->second;
            }
        }

        for (auto it = mapv.begin(); it != mapv.end(); it++)
        {
            if (it->second == max)
            {
                out += it->first;
                out += ", ";
                // if()
                //cout << prog->StringCount() << endl;
            }
        //cout << out << endl;
        }
        cout << out.substr(0, out.size() - 2) << endl;

    }
}