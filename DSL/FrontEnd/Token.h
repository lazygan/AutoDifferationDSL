//
// Created by gangan on 2021/5/21.
//

#ifndef DSL_TOKEN_H
#define DSL_TOKEN_H
#include <string>
#include <list>
#include "../dsl_utils.h"
using namespace std;
struct Token{
    string name;
    string value;
    int line;
    int column;
    Token(const string& name,const string& value,int line,int column);
    Token();
};

class TokenStream{
private:
public:
    list<Token>& tokens;
    list<Token>::iterator pos;
    TokenStream(list<Token>& tokens);
    const Token& consume_expected(const string& exp_name );
    void expect_end();
    const Token& current();
    bool is_end();
};



#endif //DSL_TOKEN_H
