//
//  ConverterNFA.h
//  Lexical Analyzer
//
//  Created by mac on 4/14/18.
//  Copyright © 2018 mac. All rights reserved.
//

#ifndef ConverterNFA_h
#define ConverterNFA_h

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
using namespace std;
class ConverterNFA{
public:
    const int epislon = 2;
    const int concate = 94;
    int statesCount = 1;
    stack<NFA*> operands;
    stack<char> operators;
    /*========================================================*/
    //helper for : stack evaluation algorithm
    bool IsOperator(char ch)
    {
        return((ch == 42) || (ch == 43)|| (ch == 124) || (ch == 40) || (ch == 41) || (ch == concate));
    };
    bool isArthmiticOrRelational(char c)
    {
        return (c == '+' || c == '*' || c == '/' || c == '-'
                || c == '=' || c == '(' || c == ')');
    }
    
    bool isInput(char prevChar ,char ch)
    {
        return !IsOperator(ch) || (prevChar == '\\' and isArthmiticOrRelational(ch));
    }
    State* getState()
    {
        return new State(statesCount++);
    }
    bool Presedence(char opLeft, char opRight)
    {
        if(opLeft == opRight)
            return true;
        if(opLeft == '*' || opLeft == '+')
            return false;
        if(opRight == '*' || opRight == '+')
            return true;
        if(opLeft == concate)
            return false;
        if(opRight == concate)//concate
            return true;
        if(opLeft == '|')
            return false;
        return true;
    };
    void pushCharNfa(char c)
    {
        State* start = new State(statesCount++);
        State* ending = new State(statesCount++);
        start->addTransition(c,ending);
        NFA* newNFA = new NFA(start,ending);
        operands.push(newNFA);
    }
    bool isRightParanthesis(char c)
    {
        return (c == ')');
    }
    bool isLeftParanthesis(char c)
    {
        return (c == '(');
    }
    
    bool popAndOperate()
    {
        if(operators.size() == 0 || operands.size() == 0)
        {
            return false ;
        }
        char operation = operators.top();
        operators.pop();
        if((operation == '|' || operation == concate ) && operands.size() <= 1)
        {
            return false;
        }
        if(operation == '+')
        {
            NFA* poped = operands.top();
            operands.pop();
            poped->positiveClosure();
            operands.push(poped);
        }
        else if(operation == '*')
        {
            int temp = statesCount;
            statesCount += 2;
            NFA* pushed = new NFA(new State(temp),new State(temp + 1));
            NFA* poped = operands.top();
            operands.pop();
            poped->kleenClosure(pushed);
            operands.push(pushed);
        }
        else if(operation == '|')
        {
            int temp = statesCount;
            statesCount += 2;
            NFA* pushed = new NFA(new State(temp),new State(temp + 1));
            NFA* poped1 = operands.top();
            operands.pop();
            NFA* poped2 = operands.top();
            operands.pop();
            poped2->unionNFA(pushed,poped1);
            operands.push(pushed);
        }
        else
        {
            int temp = statesCount;
            statesCount += 2;
            NFA* pushed = new NFA(new State(temp),new State(temp + 1));
            NFA* poped1 = operands.top();
            operands.pop();
            NFA* poped2 = operands.top();
            operands.pop();
            poped2->concateNFA(poped1,pushed);
            operands.push(pushed);
        }
        return true;
    }
    /*=======================================================*/
    bool createNFA(string& regex,string& prio,State* nfaCombined,map<string,int>& tokensPriority)
    {
        char c;
        for(int i = 0; i <(int)regex.size() ; ++i)
        {
            c = regex[i];
            if(c == '\\')continue;
            char prevChar = (i > 0)?regex[i-1] : 3;
            if(i > 0 and regex[i-1] == '\\' and  regex[i] == 'L')
                pushCharNfa(epislon);
            else if(isInput(prevChar,c))
                pushCharNfa(c);
            else if(operators.empty())
                operators.push(c);
            else if(isLeftParanthesis(c))
                operators.push(c);
            else if(isRightParanthesis(c))
            {
                while(!isLeftParanthesis(operators.top()))
                    if(!popAndOperate())
                        return false;
                operators.pop();
            }
            else
            {
                while(!operators.empty() && Presedence(c, operators.top()))
                    if(!popAndOperate())
                        return false;
                operators.push(c);
            }
        }
        while(!operators.empty())
        {
            if(!popAndOperate())
                return false;
        }
        operands.top()->getEnd()->acceptedTokenNum.insert(tokensPriority[prio]);
        nfaCombined->addTransition(epislon,operands.top()->start);
        operands.pop();
        return true;
    }
    NFA* createNFA(string& regex,int num)
    {
        char c;
        for(int i = 0; i <(int)regex.size() ; ++i)
        {
            c = regex[i];
            if(c == '\\')continue;
            char prevChar = (i > 0)?regex[i-1] : 3;
            if(i > 0 and regex[i-1] == '\\' and  regex[i] == 'L')
                pushCharNfa(epislon);
            else if(isInput(prevChar,c))
                pushCharNfa(c);
            else if(operators.empty())
                operators.push(c);
            else if(isLeftParanthesis(c))
                operators.push(c);
            else if(isRightParanthesis(c))
            {
                while(!isLeftParanthesis(operators.top()))
                    if(!popAndOperate())
                        return NULL;
                operators.pop();
            }
            else
            {
                while(!operators.empty() && Presedence(c, operators.top()))
                    if(!popAndOperate())
                        return NULL;
                operators.push(c);
            }
        }
        while(!operators.empty())
        {
            if(!popAndOperate())
                return NULL;
        }
        operands.top()->getEnd()->acceptedTokenNum.insert(num);
        NFA* res = operands.top();
        operands.pop();
        return res;
    }
    void combineNFAS(State* nfaCombined,multimap<string,string>& regx,map<string,int>& tokensPriority)
    {
        for(pair<string,string> p : regx)
        {
            createNFA(p.second, p.first,nfaCombined,tokensPriority);
        }
    }
    void cleanUp(){
        statesCount = 1;
        while(!operands.empty()){
            operands.pop();
        }
        while(!operators.empty()){
            operators.pop();
        }
    }
};
#endif /* ConverterNFA_h */
