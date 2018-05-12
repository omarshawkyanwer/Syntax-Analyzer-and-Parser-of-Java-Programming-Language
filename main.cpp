#include <iostream>
#include <string>
#include <stack>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include "NFA.h"
#include "PatternRecognizer.h"
#include "State.h"
#include "Parser.h"
#include "DFA.h"
#include "print.h"
#include "ConverterNFA.h"
#include "CFGProcessor.h"
#include "parserBuilder.h"
/*========================================================*/
multimap<string,string> regx;
map<string,int> tokensPriority;
State* nfaCombined;
vector<string> tokens;

int main(int argc, const char * argv[])
{
    Parser parseInput;
    ConverterNFA con;
    regx.clear();
    tokensPriority.clear();
    vector<string>words;
    words.clear();
    string grammarFile = "input.txt";
    string codeFile = "code.txt";
    parseInput.processLexicalFile(regx,tokensPriority,grammarFile);
    parseInput.readInputFile(words,codeFile);
    nfaCombined = new State();
    con.combineNFAS(nfaCombined,regx,tokensPriority);
    DFA* minimized = new DFA(nfaCombined);
    PatternRecognizer* pp = new PatternRecognizer(minimized->startOfDfa,minimized->endOfDfa,regx,tokensPriority);
    for(string s : words)
    {
        vector<int> par;
        par.clear();
        pp->processWord(s, par);
        for(int num : par){
            string s = "'";
            s += parseInput.getToken(num);
            s += "'";
            tokens.push_back(string(s));
        }
    }
    tokens.push_back(string("$"));
   //phase 2 starts here
    string cfgFile = "CFG.txt";
    parserBuilder* p = new parserBuilder(cfgFile);
    vector<string> output;
    output.clear();
    p->getProductions(tokens,output);
    freopen("output.txt","w",stdout);
    p->printtParserTable();
    cout<<endl<<"==========================================================="<<endl;
    for(string s : output){
        cout<<s<<endl;
    }
    cout<<endl;
    fclose(stdout);
}

