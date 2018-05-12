
#ifndef State_h
#define State_h
#include <string>
#include <stack>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
using namespace std;

class State{
public:
    int stateId;//no setter as we set it once
    
    set<State*> subset;
    multimap<char, State*> transition;
    set<int> acceptedTokenNum;//number of accepted tokens at this State
    State() : stateId(-1){};
    State(int ids){
        stateId = ids;
        transition.clear();
        acceptedTokenNum.clear();
        
    }
    State(set<State*> sub,int stateNum){
        subset = sub;
        stateId = stateNum;
        acceptedTokenNum.clear();
      
        for(State* s : sub){
            for(int token : s->acceptedTokenNum){
                acceptedTokenNum.insert(token);
            }
        }
    }
    //copy constructor
    State(const State &other){
        *this = other;
    };
    //Destructor
    ~State() {
        cout<<"destructor"<<endl;
        transition.clear();
        acceptedTokenNum.clear();
      
    };
    //getters
    int getstateId(){
        return stateId;
    }
    //to get all States we can reach from this State by a char chInput and store them in States vector
    // States : passed by reference
    void GetTransition(char chInput, vector<State*> &States)
    {
        States.clear();
        // Iterate through all values with the key chInput
        multimap<char, State*>::iterator iter;
        for(iter = transition.lower_bound(chInput);iter!= transition.upper_bound(chInput);++iter)
        {
            State* pState = iter->second;
            States.push_back(pState);
        }
    };
    bool addTransition(int chInput,State* to){
        if(!to){
            return false;
        }
        transition.insert(make_pair(chInput,to));
        return true;
    }
    //overrriding comparison operator
    void operator=(const State& other)
    {
        transition    = other.transition;
        stateId        = other.stateId;
        acceptedTokenNum     = other.acceptedTokenNum;
        subset     = other.subset;
    };
    
    //! Override the comparison operator
    bool operator==(const State& other)
    {
        if(subset.empty())
            return(stateId == other.stateId);
        else return(subset == other.subset);
    };
    //for debuging
    void printTrans(){
        multimap<char, State*>::iterator iter;
        for(iter = transition.begin();iter!= transition.end();++iter){
            cout<<stateId<<" is connected to "<<iter->second->getstateId()<<" by a character "<<(char)iter->first<<endl;
        }
        cout<<endl;
        
    }
};
#endif /* State_h */
