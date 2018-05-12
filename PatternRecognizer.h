#ifndef PatternRecognizer_h
#define PatternRecognizer_h
#include <string>
#include <stack>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include "State.h"
#include "DFA.h"
#include "NFA.h"
#include "ConverterNFA.h"


using namespace std;

class PatternRecognizer{
    private :
    State* start;
    State* current;
    State* terminal;
    unordered_map<int,State*> check;
    bool canMove(State* s,char c){
        return (s->transition.find(c)!=s->transition.end()) and   (s->transition.lower_bound(c))->second != terminal;
    }
    bool match(State* s,string& w){
        State* temp = s;
        for(char c : w){
            //never happens
            if(temp->transition.count(c) == 0){
                return false;
            }
            temp = temp->transition.lower_bound(c)->second;
        }
        return temp->acceptedTokenNum.size() > 0;
    }
public:
    PatternRecognizer(State* startDFA,State* ending,multimap<string,string>& regx,map<string,int>& tokensPrio){
        start = startDFA;
        terminal = ending;
        check.clear();
        ConverterNFA con;
        for(pair<string,string> p : regx){
            DFA* newOne = new DFA(con.createNFA(p.second,tokensPrio[p.first])->start);
            check.insert(make_pair(tokensPrio[p.first],newOne->startOfDfa));
            con.cleanUp();
        }
    }
    
    void processWord(string& word,vector<int>& tokenNums,int startIdx = 0){
        if(startIdx >= word.size()){
            return ;
        }
        current = start;
        
        int len = 0;
        int tokenNum = -1,i;
        int ending = startIdx;
        for(i = startIdx ; i < (int)word.length();++i){
            //current->printTrans();
            if(current->acceptedTokenNum.size() > 0 and i > startIdx){
                ending = i - 1;
                len = ending - startIdx + 1;
                string ss = word.substr(startIdx,len);
                for(int j : current->acceptedTokenNum){
                    if(match(check[j],ss)){
                        tokenNum = j;
                        break;
                    }
                }
            }
            if(!canMove(current,word[i]))break;
            current = current->transition.lower_bound(word[i])->second;
        }
        if(i == word.size() and (int)current->acceptedTokenNum.size() > 0){
            ending = i - 1;
            len = ending - startIdx + 1;
            string ss = word.substr(startIdx,len);
            for(int j : current->acceptedTokenNum){
                if(match(check[j],ss)){
                    tokenNum = j;
                    break;
                }
            }
            tokenNums.push_back(tokenNum);
            return ;
        }else if(i == word.size() ){
            return;
        }
        if(tokenNum == -1){
            cout<<"The character "<<word[startIdx]<<" doesn't follow any token"<<endl;
            processWord(word,tokenNums,startIdx+1);
        }else{
            tokenNums.push_back(tokenNum);
            processWord(word,tokenNums,ending + 1);
        }
    }
};

#endif /* PatternRecognizer_h */
