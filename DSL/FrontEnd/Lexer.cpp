//
// Created by gangan on 2021/5/20.
//

#include <fstream>
#include "Lexer.h"
#include <stack>



Lexer::Lexer(const string &fn):fn(fn){
    //The order imply priority
    reg_patternes={
            {"COMMENT",     regex("#.*")},
            {"NUMBER",      regex("\\d+\\.\\d+")},
            {"NUMBER",      regex("\\d+")},
            {"NAME",        regex("[a-zA-Z_]\\w*")},
            {"WHITESPACE",  regex("[ \\t]+")},
            {"NEWLINE",     regex("\\n+")},
            {"OPERATOR",    regex("[\\+\\*\\-/%]")},       // arithmetic operators
            {"OPERATOR",    regex("<=|>=|==|!=|<|>")},   // comparison operators
            {"OPERATOR",    regex("\\|\\||&&")},           // boolean operators
            {"OPERATOR",    regex("!")},                  // unary operator
            {"ASSIGN",      regex("=")},
            {"LPAREN",      regex("\\(")},
            {"RPAREN",      regex("\\)")},
            {"LBRACK",      regex("\\[")},
            {"RBRACK",      regex("\\]")},
            {"LCBRACK",     regex("\\{")},
            {"RCBRACK",     regex("\\}")},
            {"COLON",       regex(":")},
            {"COMMA",       regex(",")},
    };
    ignore_tokens={
            "WHITESPACE",
            "COMMENT"
    };
    key_words={
            {"func","FUNCTION"},
    };
}

//依靠返回值优化
list<Token> Lexer::tokenize_line(const char* aline,int str_size,int line_num) {
    list<Token> tokens_of_line;
    const char* pos=aline;

    std::cmatch cm;
    //Because limited by standary library API, the process here was not very efficient. It can be improved by trie or AC automata algorithm.
    while(true){
        bool right_step=false;
        for(pair<string,regex> p:reg_patternes) {
            regex_search(pos,cm ,p.second);
            if(cm.position()==0&&cm.length()!=0){
                if(ignore_tokens.count(p.first)==0){
                    if(p.first=="NAME"&& (key_words.count(cm.str())!=0)){
                        tokens_of_line.push_back(Token(key_words[cm.str()],"",line_num,pos-aline+cm.position()+1));
                    }else{
                        tokens_of_line.push_back(Token(p.first,cm.str(),line_num,pos-aline+cm.position()+1));
                    }
                }
                right_step=true;
                break;
            }
        }
        //if not matching any pattern
        if(!right_step){
            if( pos-aline!=str_size){
                throw SyntaxErrorException("unwanted character at  "+ to_string(line_num) +","+to_string(pos-aline));
            }else{
                break;
            }
        }
        pos+=cm.length();
    }
    if(tokens_of_line.size()!=0){
        tokens_of_line.push_back(Token("NEWLINE","",line_num,pos-aline+1));
    }
    return tokens_of_line;
}

list<Token> Lexer::tokenize() {
    if(!tokens.empty())
        return tokens;

    ifstream fs(fn) ;
    if(!fs){
        throw "fail to load file";
    }
    string s;
    int line_num=1;
    stack<int> indent_len_stack;
    indent_len_stack.push(0);
    /* func f1(x,y):
     *     func f2(x,y):
     *         func f3(x,y):
     *             1
     *         1
     *     1
     *  f1(1,2)
     *  (f1...),(INDENT),(f2...),(INDENT),(f3...),(INDENT),(1),(DEDENT),(1),(DEDENT),(1),(DEDENT),(f1...)
     */

    while(getline(fs,s)){
        int dil=_detect_indent_len(s);
        if(indent_len_stack.empty()) {
                throw SyntaxErrorException("indent error at "+to_string(line_num));
        }else{
            int last_dil=indent_len_stack.top();
            if(dil>last_dil){
                tokens.push_back(Token("INDENT","",line_num,0));
                indent_len_stack.push(dil);
            }else if(dil<last_dil){
                indent_len_stack.pop();
                if(dil!=indent_len_stack.top()){
                    throw SyntaxErrorException("indent error at "+to_string(line_num));
                }else{
                    tokens.push_back(Token("DEDENT","",line_num,0));
                }
            }
        }
        //s=s.substr(dil,-1);
        tokens.splice(tokens.end(),tokenize_line(s.c_str(),s.size(),line_num));
        line_num++;
    }
    while(!indent_len_stack.empty()){
        tokens.push_back(Token("DEDENT","",line_num,0));
        indent_len_stack.pop();
        line_num++;
    }
    tokens.pop_back();
    return tokens;
}

int Lexer::_detect_indent_len(const string &line) {
    int count=0;
    for(int i=0;i<line.size();i++) {
        if (line[i] == ' ') {
            count+=1;
        }else if (line[i] == '\t') {
            //one tab equal to 4 space
            count+=4;
        }else{
            break;
        }
    }
    return count;
}
