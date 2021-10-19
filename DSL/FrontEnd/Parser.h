//
// Created by gangan on 2021/5/21.
//

#ifndef DSL_PARSER_H
#define DSL_PARSER_H

#include "Token.h"
#include "../dsl_utils.h"
#include <functional>
#include <map>
class Parser {
private:
    map<string,int> infix_priority={
            {"[",11},
            {"(",10},
            {"*",7},
            {"/",7},
            {"+",6},
            {"-",6},
            {"<",5},
            {"<=",5},
            {">",5},
            {">=",5},
            {"==",5}
    };
    TokenStream tokens;

    // list_of_expr: (expr COMMA)*
    Pair* list_of_expr();
    // group_expr: LPAREN expr RPAREN
    Pair* group_expr_parse();
    // call_expr: NAME LPAREN list_of_expr? RPAREN
    Pair* call_expr_parse(Pair* left);
    // subscript_expr : NAME LBRACK NUMBER RBRACK
    Pair* subscript_expr_parse(Pair* left);
    //unary_op_expr: OPERATOR expr
    Pair* unary_expr();
    //binary_op_expr: expr OPERATOR expr
    Pair* binary_op_expr(Pair* left);
    //infix_expr: binary_op_expr | call_expr
    Pair* infix_expr(Pair* left);
    //prefix_expr: NUMBER |  NAME | group_expr | unary_op_expr
    Pair* prefix_expr();
    //expr: prefix_expr | infix_expr
    Pair* expr_parse(int priority=0);
    //assign_expr: expr ASSIGN expr NEWLINE
    Pair* assign_expr(Pair* left);
    // expr_stmnt:  assign_expr | expr NEWLINE
    Pair* expr_stmnt_parse();
    // func_stmnt: FUNCTION NAME LPAREN func_params? RPAREN COLON block
    Pair* fun_stmnt_parse();
    // func_params: (NAME COMMA)*
    Pair* fun_params();
    // stmnt:  expr_stmnt | func_stmnt
    Pair* stmnt_parse();
    // stmnts: stmnt*
    Pair* stmnts_parse();
    // block: NEWLINE INDENT stmnts DEDENT
    Pair* block_parse();
    // prog: stmnts
    Pair* prog_parse();
public:
    Parser(TokenStream& token_stream);
    AST parse();
};


#endif //DSL_PARSER_H
