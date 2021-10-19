//
// Created by gangan on 2021/5/20.
//

#ifndef DSL_LEXER_H
#define DSL_LEXER_H
#include <string>
#include <iostream>
#include <regex>
#include <list>
#include <set>
#include <vector>
#include <map>
#include "../dsl_utils.h"
#include "Token.h"
using namespace std;


class Lexer {
private:
    const string& fn;
    list<Token> tokens;
    vector<pair<string,regex>> reg_patternes;
    set<string> ignore_tokens;
    map<string,string> key_words;
    int _detect_indent_len(const string& line);
public:
    Lexer(const string& fn);
    //²»¿¼ÂÇindent
    list<Token> tokenize_line(const char* aline,int str_size,int line_num);
    //¿¼ÂÇindent
    list<Token> tokenize();

};




#endif //DSL_LEXER_H
