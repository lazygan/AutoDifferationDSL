//
// Created by gangan on 2021/5/25.
//

#ifndef DSL_INTERPRETORIMPLEMENTS_H
#define DSL_INTERPRETORIMPLEMENTS_H
#include "DSL/dsl_utils.h"
#include <map>
#include <set>

#include<functional>
using namespace std;
using namespace std::placeholders;

class ExprInterpretor;
class Frame;
class Procedure{
public:
    virtual CalcRes forward_calc_call(Frame& env,Pair* args= nullptr){ throw InterpretorErrorException("Not Implement");}
    virtual CalcRes forward_diff_call(Frame& env,Pair* args= nullptr){ throw InterpretorErrorException("Not Implement");}
    virtual void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr){ throw InterpretorErrorException("Not Implement");}
    virtual ~Procedure() = default;
};

class Frame{
public:
    map<string,shared_ptr<Procedure>> bindings;
    Frame* parent=nullptr;
    Frame(){ }
    Frame(Frame* parent):parent(parent){ }
    void define(string symbol,shared_ptr<Procedure> proc);
    Procedure& lookup(string symbol);
};

class ExprInterpretor {
public:
    string forward_diff_symbol;
    set<string> reverse_diff_symbol_set;
    map<string,CalcRes> reverse_diff_res;
    void reverse_proc_expr_group(Pair *root, Frame &frame, CalcRes pass_val, function<void(Pair *, Frame &,CalcRes pass_val)>proc);
    CalcRes forward_proc_expr_group(Pair *root, Frame &frame, function<CalcRes(Pair *, Frame &)> proc);
    CalcRes forward_calc_expr(Pair *root, Frame &env);
    CalcRes forward_diff_expr(Pair *root, Frame &env);
    void reverse_diff_expr(Pair *root, Frame &env,CalcRes pass_val);
    void defineSymbol(Pair *p, Frame &frame);
};

class FuncCallProcedure: public Procedure{
public:
    Pair* uncalced_body;
    Pair* procedure_args;
    ExprInterpretor& exprProcedure;
    // 这里的实现方法有点绕,主要是为了支持后续实现自动微分
    map<string,Pair*> calced_bodys;
    map<string,Frame> child_frame;
    string arg_hash(Pair* args);
    Frame& make_child_frame(Frame& env,Pair* args);
    FuncCallProcedure(Pair *procedure_body, Pair *procedure_args, ExprInterpretor& exprInterpretor);
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
    ~FuncCallProcedure();
};

class ExprProcedure : public Procedure{
public:
    Pair* body;
    Frame& env;
    ExprInterpretor& exprInterpretor;
    ExprProcedure(Pair* procedure_body,Frame& env , ExprInterpretor& expr);
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};


#endif //DSL_INTERPRETORIMPLEMENTS_H
