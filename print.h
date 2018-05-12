#ifndef PRINT_H
#define PRINT_H
#include <vector>
#include "State.h"
class print
{
public:
    void p(vector<State*> States){
        freopen("DFA table.txt","w",stdout);
        cout<<"State"<<"|| tokens accepted  |  transitions"<<endl;
        for(int i=0;i<States.size();i++){
            State* temp=States[i];
            cout<<temp->stateId<<"   ||";
            for(int t : temp->acceptedTokenNum){
                cout<<"  T:"<<t;
                
            }
            
            for(multimap<char,State*>::const_iterator it=temp->transition.begin(), end=temp->transition.end(); it!=end; ++it)
            {
                cout<<" || "<<it->first<<"->"<<it->second->stateId;
            }
            cout<<endl;
            
        }
        
        fclose(stdout);
    }
    
    
};

#endif // PRINT_H

