#ifndef DFA_h
#define DFA_h
#include <string>
#include <stack>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include "State.h"
#include "NFA.h"
#include <queue>
#include <unordered_map>
using namespace std;

class DFA{
    private :
    set<int> mark;
    
public:
    int index,index2;
    State* startOfDfa = NULL;
    State* endOfDfa = NULL;
    vector<State*> dfaStates;
    vector<State*> minimizedDFA;
    vector< vector<State*> > subClasses;
    map<State*,State*> mapper;
    map<State*,State*> rep;//here
    int dfaStatesCnt;
    void dfs(State* node){
        if(mark.find(node->getstateId()) != mark.end()){
            return;
        }
        mark.insert(node->getstateId());
        cout<<node->getstateId()<<endl;
        cout<<"////////////"<<endl;
        node->printTrans();
        cout<<"////////////"<<endl;
        for(int i : node->acceptedTokenNum){
            cout<<i<<" ";
        }
        cout<<"**********************"<<endl;
        multimap<char, State*> :: iterator itr;
        for(itr = node->transition.begin() ; itr != node->transition.end() ; itr++){
            dfs(itr->second);
        }
    }
    DFA(State* nfa){
        mark.clear();
        rep.clear();
        convertNFAtoDFA(nfa);
        startOfDfa = dfaStates[0];
        minimizaDFA();
    }
    
    void epsilonClosure(set<State*> T, set<State*> &Res)
    {
        Res.clear();
        Res = T;
        stack<State*> unprocessedStack;
        set<State*>::iterator iter;
        for(iter=T.begin(); iter!=T.end(); ++iter)
            unprocessedStack.push(*iter);
        while(!unprocessedStack.empty())
        {
            State* t = unprocessedStack.top();
            unprocessedStack.pop();
            vector<State*> epsilonStates;
            epsilonStates.clear();
            t->GetTransition(2, epsilonStates);
            for(int i=0; i<epsilonStates.size(); ++i)
            {
                State* u = epsilonStates[i];
                if(Res.find(u) == Res.end())
                {
                    Res.insert(u);
                    unprocessedStack.push(u);
                }
            }
        }
    }
    void move(char chInput, set<State*> T, set<State*> &Res)
    {
        
        Res.clear();
        set<State*>::iterator iter;
        for(iter=T.begin(); iter!=T.end(); ++iter)
        {
            State* pState = *iter;
            vector<State*> States;
            pState->GetTransition(chInput, States);
            for(int i=0; i<States.size(); ++i)
                Res.insert(States[i]);
        }
    }
    void convertNFAtoDFA(State* nfa)
    {
        
        dfaStates.clear();
        dfaStatesCnt = 0;
        vector<State*> unmarkedStates;
        set<State*> DFAStartStateSet;
        set<State*> NFAStartStateSet;
        NFAStartStateSet.insert(nfa);
        epsilonClosure(NFAStartStateSet, DFAStartStateSet);
        State *DFAStartState = new State(DFAStartStateSet, ++dfaStatesCnt);
        dfaStates.push_back(DFAStartState);
        unmarkedStates.push_back(DFAStartState);
        while(!unmarkedStates.empty())
        {
            State* processingDFAState = unmarkedStates[unmarkedStates.size()-1];
            unmarkedStates.pop_back();
            multimap<char,State*>::iterator iter;
            set<char> taken;
            taken.clear();
            for(State* s : processingDFAState->subset){
                for(iter = s->transition.begin();iter != s->transition.end();iter++){
                    if((int)iter->first != 2 and taken.find(iter->first) == taken.end()){
                        taken.insert(iter->first);
                        set<State*> MoveRes, EpsilonClosureRes;
                        move(iter->first, processingDFAState->subset, MoveRes);
                        epsilonClosure(MoveRes, EpsilonClosureRes);
                        bool bFound  = false;
                        State* remember = NULL;
                        for(State* s : dfaStates){
                            //cant compare 2 sets is a pointer comparison
                            if(s->subset == EpsilonClosureRes){
                                bFound = true;
                                remember = s;
                                break;
                            }
                        }
                        if(!bFound){
                            State* U = new State(EpsilonClosureRes, ++dfaStatesCnt);
                            unmarkedStates.push_back(U);
                            dfaStates.push_back(U);
                            processingDFAState->addTransition(iter->first, U);
                        }
                        else{
                            processingDFAState->addTransition(iter->first, remember);
                        }
                    }
                }
            }
        }
    }
    //to group the accepting states together and the others together
    void seperateState(vector<State*>& finalStates,vector<State*>& notFinal){
        finalStates.clear();
        notFinal.clear();
        for(State* s : dfaStates){
            if(s->acceptedTokenNum.size() > 0){
                finalStates.push_back(s);
            }else{
                notFinal.push_back(s);
            }
        }
    }
    void changeRepresentitive(vector<State*>& t){
        if(t.size() == 0){
            return ;
        }
        State* leader = t[0];
        rep[t[0]] = leader;
        for(int i = 1 ; i < (int)t.size();i++){
            rep[t[i]] = leader;
        }
    }
    
    void divideToSubClasses(){
        set<char> allAlph;
        allAlph.clear();
        for(State* s : dfaStates){
            multimap<char,State*>::iterator itr;
            for(itr = s->transition.begin();itr != s->transition.end() ; itr++){
                allAlph.insert(itr->first);
            }
        }
        bool ok = false;
        State* phi = new State(-5);
        for(State* s : dfaStates){
            for(char c : allAlph){
                if(s->transition.count(c) == 0){
                    ok = true;
                    s->addTransition(c,phi);
                }
            }
        }
        if(ok){
            for(char c : allAlph){
                phi->addTransition(c,phi);
            }
            dfaStates.push_back(phi);
        }
        vector<State*> acc,nonAcc;
        seperateState(acc,nonAcc);
        changeRepresentitive(acc);
        changeRepresentitive(nonAcc);
        queue<vector<State*>> q;
        q.push(acc);
        q.push(nonAcc);
        vector< pair<vector< pair<char,State*> > , State* > >  compareTuples;
        while(!q.empty()){
            compareTuples.clear();
            vector<State*> current = q.front();
            q.pop();
            for(State* s : current){
                multimap<char,State*>::iterator itr;
                vector< pair<char,State*> > tuple;
                for(itr = s->transition.begin();itr != s->transition.end();itr++){
                    if((int)itr->first == 2 ||  (int)itr->first == 0 ){
                        continue;
                    }
                    State* to = rep[itr->second];
                    //  cout<<s->getstateId()<<" will go by char "<<itr->first<<" to "<<to->getstateId()<<endl;
                    tuple.push_back(make_pair(itr->first,to));
                }
                compareTuples.push_back(make_pair(vector< pair<char,State*> >(tuple),s));
            }
            sort(compareTuples.begin(),compareTuples.end());
            int start = 0;
            int ending = 0;
            int clusters = 0;
            
            for(int i = 1 ; i < (int)compareTuples.size() ; ++i){
                if(compareTuples[i-1].first == compareTuples[i].first){
                    ending++;
                }else{
                    vector<State*> subClass;
                    subClass.clear();
                    for(int j = start ; j <= ending ; j++){
                        subClass.push_back(compareTuples[j].second);
                    }
                    changeRepresentitive(subClass);
                    q.push(vector<State*>(subClass));
                    start = ending = i;
                    clusters++;
                }
            }
            if(clusters == 0){
                vector<State*> newSub;
                newSub.clear();
                for(int i = 0 ; i < (int)compareTuples.size();++i){
                    newSub.push_back(compareTuples[i].second);
                    if(newSub.back() == startOfDfa){
                        index = (int)subClasses.size();
                    }
                    if(newSub.back() == phi){
                        index2 = (int)subClasses.size();
                    }
                }
                subClasses.push_back(vector<State*>(newSub));
            }else{
                vector<State*> subClass;
                subClass.clear();
                for(int j = start ; j <= ending ; j++){
                    subClass.push_back(compareTuples[j].second);
                }
                changeRepresentitive(subClass);
                q.push(vector<State*>(subClass));
                clusters++;
            }
        }
        
    }
    //assumming that all the states in the same subclass has the same representitive
    void minimizaDFA(){
        divideToSubClasses();
        //give representitives some numbers
        int statesCnt = 1;
        mapper.clear();
        for(int i = 0 ; i < (int)subClasses.size() ; ++i){
            State* newOne = new State(statesCnt++);
            newOne->acceptedTokenNum.clear();
            if(i == index){
                startOfDfa = newOne;
            }
            if(i == index2){
                endOfDfa = newOne;
            }
            for(State* k : subClasses[i]){
                for(int token : k ->acceptedTokenNum){
                    newOne->acceptedTokenNum.insert(token);
                }
            }
            minimizedDFA.push_back(newOne);
            mapper.insert(make_pair(rep[subClasses[i][0]],newOne));
        }
        set<char> insertedBefore;//to make it deterministic
        for(int i = 0 ; i < (int)subClasses.size() ; ++i){
            insertedBefore.clear();
            for(int j = 0 ; j < (int)subClasses[i].size();++j){
                for(multimap<char, State*>::iterator itr = subClasses[i][j]->transition.begin() ; itr != subClasses[i][j]->transition.end();itr++){
                    if(insertedBefore.find(itr->first) != insertedBefore.end()){
                        continue;
                    }
                    minimizedDFA[i]->addTransition(itr->first,mapper[rep[itr->second]]);
                    insertedBefore.insert(itr->first);
                }
            }
        }
    }
};
#endif /* DFA_h */

