//
// Created by gangan on 2021/5/21.
//

#include "Parser.h"

Parser::Parser(TokenStream& token_stream) : tokens(token_stream) { }

Pair* Parser::prog_parse() {
    Pair* stmnts=stmnts_parse();
    tokens.expect_end();
    return stmnts;
}

AST Parser::parse() {
    return AST(prog_parse());
}

Pair* Parser::stmnts_parse() {
    Pair* head=new Pair( stmnt_parse(),nullptr);
    Pair* statement=head;
    while (!tokens.is_end()){
        if(tokens.current().name == "DEDENT"){
            break;
        }
        statement->second=new Pair(stmnt_parse(), nullptr);
        statement=statement->second;
    }
    return head;
}

Pair* Parser::stmnt_parse() {
    string n=tokens.current().name;
    if(n=="FUNCTION") return fun_stmnt_parse();
    return expr_stmnt_parse();
}

Pair* Parser::fun_stmnt_parse() {
    tokens.consume_expected("FUNCTION");
    const Token& id_token=tokens.consume_expected("NAME");
    tokens.consume_expected("LPAREN");
    Pair* args=fun_params();
    tokens.consume_expected("RPAREN");
    tokens.consume_expected("COLON");
    string n=tokens.current().name;
    Pair* block=block_parse();
    return new Pair(
            new Pair("define"),
                new Pair(
                    new Pair(
                        new Pair(id_token.value),
                        args),
                    block));
}

Pair* Parser::assign_expr(Pair* left) {
    tokens.consume_expected("ASSIGN");
    Pair* right=expr_parse();
    tokens.consume_expected("NEWLINE");
    return new Pair(new Pair("define"),new Pair(left, new Pair(right,nullptr)));
}


// parse func_params: (NAME COMMA)*  first
Pair* Parser::fun_params() {
    Pair* head=nullptr;
    Pair* p;
    if(tokens.current().name == "NAME"){
        while(!tokens.is_end()){
            const Token& id_token=tokens.consume_expected("NAME");
            if(head==nullptr){
                head=new Pair(new Pair(id_token.value),nullptr);
                p=head;
            }else{
                p->second=new Pair(new Pair(id_token.value),nullptr);
                p=p->second;
            }
            if(tokens.current().name=="COMMA"){
                tokens.consume_expected("COMMA");
            }else{
                return head;
            }
        }
    }
    return nullptr;
}

// block: NEWLINE INDENT stmnts DEDENT
Pair *Parser::block_parse() {
    tokens.consume_expected("NEWLINE");
    string n=tokens.current().name;
    tokens.consume_expected("INDENT");
    Pair* stmnts=stmnts_parse();
    tokens.consume_expected("DEDENT");
    return stmnts;
}



Pair *Parser::unary_expr() {
    const Token& t= tokens.consume_expected("OPERATOR");
    if(t.value!="-")
        throw SyntaxErrorException(t.line,t.column,"Unary operator {"+ t.value+"} not supported");
    Pair* right=expr_parse();
    if(right==nullptr)
        throw SyntaxErrorException(t.line,t.column,"Expected expression");
    return new Pair(
            new Pair("-"),
            new Pair(
                    right,
                    nullptr)
            );
}

Pair *Parser::expr_parse(int priority) {
    Pair* left = prefix_expr();
    if(left!=nullptr){
        while(priority<infix_priority[tokens.current().value]){
            //"(0)1+2+3+(1)4*5+1" 括号位置表示优先级变动,
            Pair* greater_than_this_priority_expr=infix_expr(left);
            if(greater_than_this_priority_expr!=nullptr)
                left=greater_than_this_priority_expr;
        }
        return left;
    }
    return nullptr;
}

Pair *Parser::prefix_expr() {
    const Token& token=tokens.current();
    string n=tokens.current().name;
    if(n=="NAME"){
        tokens.consume_expected("NAME");
        return new Pair(token.value);
    }
    if(n=="NUMBER"){
        tokens.consume_expected("NUMBER");
        return new Pair(stod(token.value));
    }
    if(n=="LPAREN"){ return group_expr_parse(); }
    if(n=="OPERATOR"){ return unary_expr(); }
    return nullptr;
}

Pair *Parser::group_expr_parse() {
    tokens.consume_expected("LPAREN");
    Pair* right=expr_parse();
    tokens.consume_expected("RPAREN");
    return right;
}

Pair *Parser::binary_op_expr(Pair* left) {
    const Token& token = tokens.consume_expected("OPERATOR");
    Pair* right = expr_parse(infix_priority[token.value]);
    if(right){
        return new Pair(
                new Pair(token.value),
                new Pair( left,
                           new Pair(right,nullptr)));
    }
    throw SyntaxErrorException(token.line,token.column,"Expected Expression");
}

Pair *Parser::infix_expr(Pair *left) {
    string n=tokens.current().name;
    if(n=="OPERATOR"){ return binary_op_expr(left); }
    if(n=="LPAREN"){ return call_expr_parse(left); }
    if(n=="LBRACK"){ return subscript_expr_parse(left); }
    return nullptr;
}

Pair *Parser::call_expr_parse(Pair *left) {
    tokens.consume_expected("LPAREN");
    Pair* argument=list_of_expr();
    tokens.consume_expected("RPAREN");
    return new Pair(left,argument);
}

Pair *Parser::expr_stmnt_parse() {
    Pair* expr=expr_parse();
    if(expr){
        if(tokens.current().name == "ASSIGN"){
            return assign_expr(expr);
        }
        else{
            tokens.consume_expected("NEWLINE");
            return expr;
        }
    }
    return nullptr;
}

Pair *Parser::list_of_expr() {
    Pair* head=nullptr;
    Pair* p=nullptr;
    while(!tokens.is_end()){
        Pair* expr = expr_parse();
        if(expr) {
            if (head==nullptr) {
                head = new Pair(expr, nullptr);
                p = head;
            } else {
                p->second = new Pair(expr, nullptr);
                p = p->second;
            }
        }
        else
            return head;
        if(tokens.current().name=="COMMA") {
            tokens.consume_expected("COMMA");
        }else {
            return head;
        }
    }
    return nullptr;
}

Pair *Parser::subscript_expr_parse(Pair *left) {
    tokens.consume_expected("LBRACK");
    Pair* argument=list_of_expr();
    argument->second=new Pair(left,nullptr);
    tokens.consume_expected("RBRACK");
    return new Pair(new Pair("subscript"),argument);
}

