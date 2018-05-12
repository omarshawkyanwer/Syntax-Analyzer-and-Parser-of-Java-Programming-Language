#ifndef NFA_h
#define NFA_h
#include <string>
#include <stack>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include "State.h"
using namespace std;
const int epsilon = 2;
class NFA{
public :
    State* start;//start / end node for the nfa
    State* ending;
    //default constructors : make them pointing to zero --> null in  cpp
    //parametrized constructor
    NFA(State* start,State* ending) : start(start) , ending(ending){}
    //copy constructor
    NFA(const NFA &other){
        *this = other;
    };
    //Destructor
    ~NFA(){};
    //need new state other
    void unionNFA(NFA* nfa,NFA* other){
        (nfa->getStart())->addTransition(epsilon,start);
        (nfa->getStart())->addTransition(epsilon,other->getStart());
        ending->addTransition(epsilon,nfa->getEnd());
        (other->getEnd())->addTransition(epsilon,nfa->getEnd());
        (nfa->getEnd())->acceptedTokenNum.clear();
        merge((ending->acceptedTokenNum).begin(), (ending->acceptedTokenNum).end(),
        (other->getEnd())->acceptedTokenNum.begin(),(other->getEnd())->acceptedTokenNum.end(),
        inserter((nfa->getEnd())->acceptedTokenNum, (nfa->getEnd())->acceptedTokenNum.end()));
        (ending->acceptedTokenNum).clear();
        (other->getEnd())->acceptedTokenNum.clear();
        ending->acceptedTokenNum.clear();
    }
    //concate this with other
    void concateNFA(NFA* other,NFA* nfa){
        ending->addTransition(epsilon,other->getStart());
        nfa->getStart()->addTransition(epsilon,start);
        other->getEnd()->addTransition(epsilon,nfa->getEnd());
        nfa->getEnd()->acceptedTokenNum = other->getEnd()->acceptedTokenNum;
        other->getEnd()->acceptedTokenNum.clear();
    }
    //need a new state
    void kleenClosure(NFA* nfa){
        (nfa->getStart())->addTransition(epsilon,nfa->getEnd());
        ending->addTransition(epsilon, start);
        (nfa->getStart())->addTransition(epsilon, start);
        ending->addTransition(epsilon,nfa->getEnd());
        (nfa->getEnd())->acceptedTokenNum.clear();
        (nfa->getEnd())->acceptedTokenNum.insert((ending->acceptedTokenNum).begin(),
        (ending->acceptedTokenNum).end());
        (ending->acceptedTokenNum).clear();
    }
    //no need for a new state
    void positiveClosure(){
        ending->addTransition(epsilon,start);
    }
    //getters
    State* getStart(){
        return start;
    }
    State* getEnd(){
        return ending;
    }
    void operator=(const NFA& other){
        ending = other.ending;
        start = other.start;
    }
    //setter
};
#endif /* NFA_h */
