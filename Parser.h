#ifndef Parser_h
#define Parser_h
#include <string>
#include <cstring>
#include <stack>
#include <map>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>
#include "State.h"

using namespace std;

class Parser{
    private :
    unordered_map<string, string> definitions;//to substitute later in regex
    unordered_map<string, string> expressions;
    map<int,string> tokens;
    
    bool first_line = true;
    int counter = 30;
    int countFirst = 5;
    
    
    void removeBlanks(string& s){
        s.erase(remove(s.begin(), s.end(), ' '), s.end());
        s.erase(remove(s.begin(), s.end(), '\t'), s.end());
    }
    
    string decomposeString(string& s){
        string res = "(";
        for(int i = s[0]; i <= s[2]; i++){
            res += (char)i;
            if(i != s[2])
                res += "|";
        }
        res += ")";
        return res;
    }
    
    string addConcateOperation(string& regx){
        string res = "";
        for(int i = 0; i < regx.length(); i++){
            res += regx[i];
            if(i != regx.length() - 1)
                res += (char)94;
        }
        return res;
    }
    
    string concatInExpr(string& regx){
        string res = "";
        for(int i = 0; i < regx.length(); i++){
            if(regx[i] == '(' && i != 0 && i-1 >= 0 && regx[i-1] != '*' && regx[i-1] != '+' && regx[i-1] != '|') {
                res += (char)94;
                res += regx[i];
            } else if(regx[i] == '.'){
                res += (char)94;
                res += regx[i];
                res += (char)94;
            } else if ((regx[i] == '*'|| regx[i] == '+') && i + 1 < regx.length() && regx[i+1] != '|' && regx[i+1] != ')' && regx[i+1] !='.'){
                res += regx[i];
                res += (char)94;
            }else if(regx[i] == 'E'){
                res += regx[i];
                res += (char)94;
            }else if(regx[i] == ')' && i + 1 < regx.length() && regx[i+1] != '|' && regx[i+1] != '*' && regx[i+1] != '+' && regx[i+1] != '('){
                res += regx[i];
                res += (char)94;
            }else if(i+1 < regx.length() && regx[i+1] == '\\' && regx[i] != '|' && regx[i] != '('){
                res += regx[i];
                res += (char)94;
                res += regx[i+1];
                i++;
            }else
                res += regx[i];
        }
        return res;
    }
    
    void delimitingString(map<string,int>& priorities, string& key, string& read, string delimiter, multimap<string,string>& tokenNameRegx, bool use, bool extra){
        size_t pos = 0;
        string token, value;
        if(use == true && extra == false){
            while ((pos = read.find(delimiter)) != std::string::npos) {
                token = read.substr(0, pos);
                priorities.insert(make_pair(token, countFirst));
                tokens.insert(make_pair(countFirst, token));
                countFirst++;
                value = addConcateOperation(token);
                tokenNameRegx.insert(make_pair(token, value));
                read.erase(0, pos + delimiter.length());
            }
        } else if(use == false && extra == false){
            while ((pos = read.find(delimiter)) != std::string::npos) {
                token = read.substr(0, pos);
                value = decomposeString(token);
                if(expressions[key].compare("")){
                    expressions[key] += "|";
                    expressions[key].erase(remove(expressions[key].begin(), expressions[key].end(), ')'),
                                           expressions[key].end());
                    value.erase(remove(value.begin(), value.end(), '('),value.end());
                    expressions[key] += value;
                    
                } else
                    expressions[key] += value;
                read.erase(0, pos + delimiter.length());
                
            }
        } else if (extra == true && use == false){
            while ((pos = read.find(delimiter)) != std::string::npos) {
                token = read.substr(0, pos);
                priorities.insert(make_pair(token, counter));
                tokens.insert(make_pair(counter, token));
                counter++;
                tokenNameRegx.insert(make_pair(token, token));
                read.erase(0, pos + delimiter.length());
            }
        }
    }
    
    
    string replaceWord(string& line){
        unordered_map<string, string>::const_iterator got;
        string newLine, res;
        int i = 0;
        while (i < line.length() || res.compare("")) {
            if(i < line.length() && (line[i] == (char)94 || line[i] == '|' || line[i] == '+'
                                     || line[i] == '*' || line[i] == ' '|| line[i] == '(' || line[i] == ')')) {
                got = expressions.find (res);
                if(got != expressions.end()) {
                    newLine += got -> second;
                    newLine += line[i];
                } else {
                    res += line[i];
                    newLine += res;
                }
                res = "";
            } else if(i >= line.length()) {
                got = expressions.find (res);
                if(got != expressions.end()) {
                    newLine += got -> second;
                    newLine += line[i];
                    res = "";
                } else {
                    break;
                }
            } else {
                res += line[i];
            }
            i++;
        }
        newLine += res;
        // //cout<<newLine<<'\n';
        return newLine;
    }
    
    
    public :
    /*
     input :
     -lexicalFileName
     output :
     -tokenNameRegx
     -priorities
     */
    void processLexicalFile(multimap<string,string>& tokenNameRegx,map<string,int>& priorities,string& fileLexicalName){
        ifstream file(fileLexicalName);
        if(file.is_open()){
            string read;
            while(getline(file, read)){
              
                if(read[0] != '{' && read[0] != '[')
                    removeBlanks(read);
                if(first_line){
                   
                    read = read.substr(3);
                    first_line = false;
                }
                string key = "";
                for(int i = 0; i < read.length(); i++){
                    string value;
                    if(read[i] == ':'){
                        value = read.substr(i+1);
                        priorities.insert(make_pair(key, counter));
                        tokens.insert(make_pair(counter, key));
                        counter++;
                        value = concatInExpr(value);
                        value = replaceWord(value);
                        tokenNameRegx.insert(make_pair(key, value));
                        break;
                        
                    } else if (read[i] == '='){
                        value = read.substr(i+1);
                        if(value.find("-") != -1){
                            value += "|";
                            string delim = "|";
                            expressions.insert(make_pair(key, ""));
                            delimitingString(priorities, key, value, delim, tokenNameRegx, false, false);
                        } else {
                            value = replaceWord(value);
                            expressions.insert(make_pair(key, value));
                        }
                        break;
                        
                    }else if (read[i] == '{' && i == 0){
                        read = read.substr(i+1);
                        read.replace(read.find("}"), 1,"");
                        if(read[read.length()-1] != ' ')
                            read += " ";
                        string delim = " ";
                        delimitingString(priorities, key, read, delim, tokenNameRegx, true, false);
                        break;
                        
                    }else if (read[i] == '[' && i == 0){
                        read = read.substr(i+1);
                        read.replace(read.find("]"), 1,"");
                        if(read[read.length()-1] != ' ')
                            read += " ";
                        string delim = " ";
                        delimitingString(priorities, key, read, delim, tokenNameRegx, false, true);
                        break;
                        
                    }else {
                        key += read[i];
                    }
                }
               
            }
            
            //   cout<<"Token and its regx:"<<'\n';
            for(multimap<string,string>::const_iterator it = tokenNameRegx.begin();
                it != tokenNameRegx.end(); ++it)
            {
                //      cout << it->first << " " << it->second<<"\n";
            }
            // cout<<"inverse table of priority"<<'\n';
            for( map<string, int>::const_iterator it = priorities.begin();
                it != priorities.end(); ++it)
            {
                //  cout << it->first << " " << it->second<<"\n";
            }
            file.close();
        } else {
            //cout<<"Error in opening file\n";
        }
    }
    
    void find_and_replace(string& source, string const& find, string const& replace)
    {
        for(string::size_type i = 0; (i = source.find(find, i)) != string::npos;)
        {
            source.replace(i, find.length(), replace);
            i += replace.length();
        }
    }
    
    
    string getToken (int priority){
        string s=tokens.find(priority)->second;
        string s2 = "\\";
        string s3 = "";
        find_and_replace(s,s2,s3);
        return s;
    }
    
    void readInputFile (vector<string>& inputFile, string& inputFileName){
        ifstream file(inputFileName);
        if(file.is_open()){
            string read;
            while(getline(file, read)) {
                read += " ";
                string delim = " ";
                string token;
                size_t pos = 0;
                while ((pos = read.find(delim)) != std::string::npos) {
                    token = read.substr(0, pos);
                    removeBlanks(token);
                    inputFile.push_back(token);
                    read.erase(0, pos + delim.length());
                }
            }
        } else {
            //cout<<"Error opening the file"<<'\n';
        }
    }
    
};


#endif /* Parser_h */

