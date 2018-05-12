
//
//  parserBuilder.h
//  Syntax Analyzer
//
//  Created by mac on 4/17/18.
//  Copyright © 2018 mac. All rights reserved.
//

#ifndef parserBuilder_h
#define parserBuilder_h
#include<set>
#include<unordered_set>
#include<unordered_map>
#include<map>
#include<cmath>
#include<string>
#include<vector>
#include<queue>
#include<stack>
#include<algorithm>
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<iostream>
#include "CFGProcessor.h"
using namespace std;
// here parser table will be built



class parserBuilder{
public:
    unordered_map<string,unordered_map<string,string>> table;
    unordered_map<string,unordered_map<string,bool>> isSync;
    CFGProcessor* helper;
    parserBuilder(string fileName){
        helper = new CFGProcessor(fileName);
        buildParserTable();
        markSync();
    }
    ~parserBuilder(){
        delete helper;
    }
    void buildParserTable(){
        string epsilonStr("\\L");
        table.clear();
        for(string nT : helper->nonTerminals){
            vector<string> followSet = helper->follow(nT);
            for(string ored : helper->split[nT]){
                bool epsilonExists = false;
                vector<string> firstSet = helper->first(ored);
                for(string ter : firstSet){
                    if(!ter.compare(epsilonStr)){
                        epsilonExists = true;
                        continue;
                    }
                    // if the entry in the parse table exists and has a different production
                    if(table[nT].find(ter) != table[nT].end() and table[nT][ter].compare(ored)){
                        cout<<"The grammer is ambigous."<<endl;
                        exit(1);
                    }else{
                        //insert normally
                        table[nT].insert(make_pair(string(ter),string(ored)));
                    }
                }
                if(epsilonExists){
                    for(string ter : followSet){
                        if(table[nT].find(ter) != table[nT].end() and table[nT][ter].compare(epsilonStr)){
                            cout<<"The grammer is ambigous."<<endl;
                            exit(1);
                        }else{
                            table[nT].insert(make_pair(string(ter),string(epsilonStr)));
                        }
                    }
                }
            }
        }
    }
    void printtParserTable(){
        
        for(pair<string,unordered_map<string,string>> p : table){
            cout<<p.first<<" : ";
            for(pair<string,string> pp : p.second){
                cout<<p.first<<" --"<<pp.first<<"--> "<<pp.second<<"    ";
            }
            cout<<endl;
        }
    }
    void markSync(){
        isSync.clear();
        for(string nT : helper->nonTerminals){
            vector<string> followSet = helper->follow(nT);
            for(string ter : followSet){
                isSync[nT].insert(make_pair(ter,true));
            }
        }
    }
    void splitBySpacesAndReverse(string& to_split ,vector<string>& output){
        output.clear();
        size_t pos = 0, found;
        while((found = to_split.find_first_of(' ', pos)) != string::npos) {
            output.push_back(to_split.substr(pos, found - pos));
            pos = found+1;
        }
        output.push_back(to_split.substr(pos));
        reverse(output.begin(),output.end());
    }
    
    //this uses panic-mode error recovery
    bool getProductions(vector<string>& input,vector<string>& output){
        string epsilonStr("\\L");
        stack<string> stk;
        stk.push(string("$"));
        stk.push(string(helper->startSymbol));
        string endSymbole = "$";
        int tokensPtr;
        bool correct = true;
        for(tokensPtr = 0;tokensPtr < (int)input.size();){
            //both are terminals
            string temp = string(stk.top());
            if(!temp.compare(epsilonStr)){
                stk.pop();
                continue;
            }
            if(!temp.compare(endSymbole)){
                return correct;
            }else if(helper->isTerminal(temp) and !input[tokensPtr].compare(temp)){
                tokensPtr++;
                string add = "Match is found in token ";
                add += stk.top();
                stk.pop();
                output.push_back(string(add));
            }else if(helper->isNonTerminal(temp) and table[temp].find(input[tokensPtr]) != table[temp].end()){
                stk.pop();
                string newProduction = string(temp);
                newProduction += "-> ";
                if(epsilonStr.compare(table[temp][input[tokensPtr]])){
                    newProduction += table[temp][input[tokensPtr]];
                }else{
                    newProduction += "EPS";
                }
                
                output.push_back(string(newProduction));
                vector<string> added;
                splitBySpacesAndReverse(table[temp][input[tokensPtr]],added);
                for(string s : added){
                    stk.push(string(s));
                }
            }
            //start panic-mode error recovery
            else if(helper->isTerminal(temp)){
                output.push_back(string("Error : unmatched terminal is inserted."));
                stk.pop();
                correct = false;
            }else{//sure the top of stack is a non-terminal
                if(isSync[temp][input[tokensPtr]]){
                    output.push_back(string("Error : sync entry is encountered."));
                    stk.pop();
                }else{//no entry found in parse table
                    string add = "Error : Skip input terminal ";
                    add += input[tokensPtr];
                    output.push_back(string(add));
                    tokensPtr++;
                }
                correct = false;
            }
        }
        return correct;
    }
};
#endif /* parserBuilder_h */
