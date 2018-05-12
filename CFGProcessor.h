
//
//  CFGProcessor .h
//  Syntax Analyzer
//
//  Created by mac on 4/17/18.
//  Copyright � 2018 mac. All rights reserved.
//

#ifndef CFGProcessor__h
#define CFGProcessor__h
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
#include <fstream>
#include <sstream>

using namespace std;
/*
 important notes :
 say we have :  STATEMENT_LIST = STATEMENT | STATEMENT_LIST  STATEMENT
 ---> input of follow function is only a non_Terminal
 ex : follow("STATEMENT_LIST");
 
 ---> input of first is an ored item
 ex : first("STATEMENT_LIST  STATEMENT");
 so internaly you have to split and get the first(STATEMENT_LIST) , if it has epsilon first(get STATEMENT) and so on
 
 
 **** best idea is to store the first of everything at after scanning i/p directly ,
 instead of computing the first of something more than once so that you can use directly without recomputing it.
 
 
 terminals ===> num , id , , , addop , mulop ........
 "addopp" ---> {"addopp"}
 ..............
 statment --->{,,,,,}
 ------------------------
 Term = Statement 'addopp'
 
 Statement_List Statment
 
 
 ===========================================================================================================
 */
class CFGProcessor{
public:
    //to store the RHS of each production
    vector<string> nonTerminals;
    unordered_map<string,vector<string>> split;
    unordered_map<string,set<string>> firstValues;
    //to save start symbol
    string startSymbol;
    bool first_line;
    
    CFGProcessor(string cfgFileName){
        ifstream file(cfgFileName);
        first_line = true;
        if(file.is_open()){
            string read;
            getline(file, read);
            removeSpaces(read);
            string line = read;
            while(getline(file, read)){
                removeSpaces(read);
                if(read[0] == '|')
                    line += read;
                else {
                    parseLine(line);
                    line = read;
                }
            }
            parseLine(line);
            for(unordered_map<string,vector<string>>::const_iterator it = split.begin();
                it != split.end(); ++it)
                first_fill(it->first);
            for(unordered_map<string,set<string>>::const_iterator it = firstValues.begin();
                it != firstValues.end(); ++it)
            {
                //cout << it->first << "----->";
                for (set<string>::iterator i = it->second.begin(); i != it->second.end(); ++i)
                    //cout << *i << " ";
                    cout<<endl;
            }
            
        } else {
            cout<<"Error in opening file\n";
        }
    }
    
    void removeSpaces(string& s) {
        int i = 0;
        while(s[i] == ' ' || s[i] == '\t' || s[i] == '#') {
            i++;
        }
        s.erase(0, i);
        i = (int)s.size() - 1;
        if(i > 0) {
            while(s[i] == ' ' || s[i] == '\t' || s[i] == '#') {
                i--;
            }
            i++;
            s.erase(i, s.size()-1);
        }
    }
    
    void parseLine(string read) {
        string delim = "=";
        string token;
        string rest;
        size_t pos = 0;
        if ((pos = read.find(delim)) != std::string::npos) {
            token = read.substr(0, pos);
            token.erase(remove(token.begin(), token.end(), ' '), token.end());
            rest = read.substr(pos + 1, read.length()-1);
            nonTerminals.push_back(token);
            if(first_line) {
                startSymbol = token;
                first_line = false;
            }
            read.erase(0, pos + delim.length());
            stringstream ss(rest);
            vector<string> parts;
            while(getline(ss, rest, '|')){
                removeSpaces(rest);
                parts.push_back(rest);
            }
            split.insert(make_pair(token, parts));
        }
    }
    
    ~CFGProcessor(){}
    
    //takes terminal or non terminal ----> give a set of terminals
    vector<string> first(string input){
        vector<string> res;
        stringstream ss(input);
        getline(ss, input, ' ');
        if(!isNonTerminal(input)){
            res.push_back(input);
        } else {
            for(set<string>::iterator it = firstValues[input].begin(); it != firstValues[input].end(); ++it)
            {
                string word = *it;
                res.push_back(word);
            }
        }
        return res;
    }
    
    void first_fill(string start) {
        vector<string> productions = split[start];
        set<string> setOfFirsts;
        setOfFirsts.clear();
        for(vector<string>::const_iterator i = productions.begin(); i != productions.end(); ++i)
        {
            string word = *i;
            removeSpaces(word);
            stringstream ss(word);
            while(getline(ss, word, ' ')) {
                if(isNonTerminal(word)) {
                    first_fill(word);
                    for(set<string>::iterator it = firstValues[word].begin(); it != firstValues[word].end(); ++it)
                    {
                        string word = *it;
                        setOfFirsts.insert(word);
                    }
                    if(firstValues[word].find("\\L") == firstValues[word].end())
                        break;
                } else {
                    setOfFirsts.insert(word);
                    break;
                }
            }
        }
        if(firstValues.find(start) != firstValues.end()) {
            for(set<string>::iterator i = setOfFirsts.begin(); i != setOfFirsts.end(); ++i)
            {
                string word = *i;
                firstValues[start].insert(word);
            }
        } else {
            firstValues.insert(make_pair(start, setOfFirsts));
        }
    }
    
    vector<string> follow(string input){
        vector<string> res;
        set<string> fset;
        fset.clear();
        
        res = process_follow(input);
        for(string s : res){
            if(s.compare("\\L")){
                fset.insert(s);
            }
        }
        res.clear();
        for(string s: fset){
            res.push_back(s);
        }
        return res;
    }
    
    vector<string> process_follow (string input) {
        vector<string> res;
        res.clear();
        if(!input.compare(startSymbol))
            res.push_back("$");
        for(unordered_map<string,vector<string>>::const_iterator it = split.begin();
            it != split.end(); ++it) {
            for (vector<string>::const_iterator i = it->second.begin(); i != it->second.end(); ++i) {
                string word = *i;
                stringstream ss(word);
                vector<string> v;
                while(getline(ss, word, ' ')){
                    v.push_back(word);
                }
                for (vector<string>::const_iterator itr = v.begin(); itr != v.end(); ++itr) {
                    string s = *itr;
                    vector<string>::const_iterator j = itr + 1;
                    
                    if(!s.compare(input)){
                        if(j == v.end()){
                            if(!s.compare(it->first)) {
                                continue;
                            }
                            vector<string> v = follow(it->first);
                            res.insert(res.end(), v.begin(), v.end());
                        }else {
                            string next = *(j);
                            vector<string> vec = first(next);
                            res.insert(res.end(), vec.begin(), vec.end());
                            
                            while(find(vec.begin(), vec.end(),"\\L")!= vec.end() && next.compare(it->first)) {
                                if((++j) == v.end()) {
                                    vec = follow(it->first);
                                    res.insert(res.end(), vec.begin(), vec.end());
                                    vec.clear();
                                } else {
                                    vec = first(*(++j));
                                    res.insert(res.end(), vec.begin(), vec.end());
                                }
                            }
                        }
                    }
                }
            }
        }
        return res;
    }
    
    bool isNonTerminal(string word){
        for(string s:nonTerminals){
            if(!s.compare(word))
                return 1;
        }
        return 0;
    }
    bool isTerminal(string word){
        return !isNonTerminal(word);
    }

};

#endif /* CFGProcessor__h */

