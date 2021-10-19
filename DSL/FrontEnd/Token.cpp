//
// Created by gangan on 2021/5/21.
//

#include "Token.h"

Token::Token(const string &name, const string &value, int line, int column):name(name),value(value),line(line),column(column) { }

Token::Token() { }
TokenStream::TokenStream(list<Token> &tokens):tokens(tokens),pos(tokens.begin()) { }


const Token &TokenStream::consume_expected(const string& exp_name) {
    const Token& token=current();
    if(token.name!=exp_name)
        throw SyntaxErrorException(token.line,token.column,"Expected "+exp_name+",but get "+token.name);
    pos++;
    return token;
}

void TokenStream::expect_end() {
    if(!is_end()){
        throw SyntaxErrorException("Expected end at  " + to_string(pos->line)+","+to_string(pos->column));
    }
}

const Token &TokenStream::current() {
     if(is_end()){
         const Token& t=tokens.back();
         throw SyntaxErrorException("Unexpected end fo input at " + to_string(t.line)+","+to_string(t.column));
     }
    return *pos;
}

bool TokenStream::is_end() {
    return pos==tokens.end();
}
