//
// Created by gangan on 2021/5/25.
//

#include "InterpretorImplements.h"
#include<vector>


void ExprInterpretor::defineSymbol(Pair *expression, Frame &env) {
    if(expression->len()<2) throw InterpretorErrorException();
    Pair* symbol=expression->first;
    Pair* formals=symbol->second;
    Pair* body=expression->second;
    auto lambda_procedure=make_shared<FuncCallProcedure>(body, formals,*this);
    if(symbol->is_symbol()) {
        //x=1 或 x=1+2+3 都视作无参数函数
        //此时formal = nullptr
        env.define(symbol->getSymbol(),lambda_procedure);
    }else if(symbol->is_node() && symbol->first->is_symbol()){
        // func x(a,b) :
        //          1+2+3
        env.define(symbol->first->getSymbol(),lambda_procedure);
    }else{
        throw InterpretorErrorException("No symbol "+ (symbol->is_node() ? symbol->first->getSymbol() :symbol->getSymbol()));
    }
}

CalcRes ExprInterpretor::forward_proc_expr_group(Pair* root, Frame& frame, function<CalcRes(Pair*, Frame&)> proc){
    if(root==nullptr) return CalcRes();
        //只在结尾处返回值有效
    while (root->second) {
            proc(root->first, frame);
            root = root->second;
        }
    return proc(root->first, frame);
}

void ExprInterpretor:: reverse_proc_expr_group(Pair *root, Frame &frame, CalcRes pass_val, function<void(Pair *, Frame &, CalcRes pass_val)>proc) {
    if(root==nullptr) return ;
    while (root->second) {
        proc(root->first, frame,pass_val);
        root = root->second;
    }
    proc(root->first, frame,pass_val);
}

CalcRes ExprInterpretor::forward_calc_expr(Pair *root, Frame& env) {
    if(root == nullptr){ throw InterpretorErrorException("empty expr"); }
    if(root->is_calced()){
        return root->get_forward_calcres();
    }
    if(root->is_number()){
        //case 1. number
        return root->set_forward_calcres(CalcRes(root->getNumber()));
    }else if(root->is_symbol()){
        //case 2. symbol
        Procedure&  proc=env.lookup(root->getSymbol());
        return root->set_forward_calcres(proc.forward_calc_call(env,nullptr));
    }else{
        //case 3. P(symbol,P(arg1,P(arg2,...(root指向第一个P)
        Pair* first= root->first; Pair* second= root->second;
        if(first->is_symbol()){
            if(first->getSymbol()=="define"){
                defineSymbol(second, env);
                return CalcRes();
            }else{
                Procedure&  proc=env.lookup(first->getSymbol());
                //递归求解参数
                forward_proc_expr_group(second, env, std::bind(&ExprInterpretor::forward_calc_expr, this, _1, _2));
                return root->set_forward_calcres(proc.forward_calc_call(env,second));
            }
        }
    }
    throw InterpretorErrorException();
}


CalcRes FuncCallProcedure::forward_calc_call(Frame& env,Pair *args) {
    if(calced_bodys.count(arg_hash(args)) == 0){
        calced_bodys[arg_hash((args))]=uncalced_body->clone_pair();
        Pair* p=calced_bodys[arg_hash(args)];
        Frame& new_env=make_child_frame(env,args);
        return exprProcedure.forward_proc_expr_group(p, new_env, std::bind(&ExprInterpretor::forward_calc_expr, &exprProcedure, _1, _2));
    }else{
        //避免重复计算
        Pair* p=calced_bodys[arg_hash(args)];
        //避免重复define 否则可用:exprProcedure.proc_expr_group(p, new_env,std::bind(&ExprProcedure::forward_calc_expr,&exprProcedure,_1,_2));
        while(p->first && p->first->first && p->first->first->getSymbol()=="define"){ p=p->second; }
        return p->is_node() ? p->first->get_forward_calcres() : p->get_forward_calcres();
    }
    throw InterpretorErrorException();
}

CalcRes ExprInterpretor::forward_diff_expr(Pair *root, Frame &env) {
    if(root == nullptr){ throw InterpretorErrorException(); }
    if(root->is_number()){
    //    //case 1. number
        return root->set_forward_diffres(CalcRes(0.0));
    }else if(root->is_symbol()){
        //case 2. symbol
        if(root->getSymbol()==forward_diff_symbol){
            return root->set_forward_diffres(root->get_forward_calcres().fill(1.0));
        }
        Procedure&  proc=env.lookup(root->getSymbol());
        return root->set_forward_diffres(proc.forward_diff_call(env,nullptr));
    }else{
       //case 3. P(symbol,P(arg1,P(arg2,...(root指向第一个P)
        Pair* first= root->first; Pair* second= root->second;
        if(first->is_symbol()){
            if(first->getSymbol()=="define"){
                return CalcRes();
            }else{
                Procedure&  proc=env.lookup(first->getSymbol());
                //递归求解参数
                forward_proc_expr_group(second, env, std::bind(&ExprInterpretor::forward_diff_expr, this, _1, _2));
                return root->set_forward_diffres(proc.forward_diff_call(env,second));
            }
        }
    }
    throw InterpretorErrorException();
}

CalcRes FuncCallProcedure::forward_diff_call(Frame& env,Pair *args) {
    string arghash=arg_hash(args);
    Pair* p=calced_bodys[arghash];
    Frame& new_env=child_frame[arghash];
    return exprProcedure.forward_proc_expr_group(p, new_env, std::bind(&ExprInterpretor::forward_diff_expr, &exprProcedure, _1, _2));
}

void FuncCallProcedure::reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair *args) {
    string arghash=arg_hash(args);
    Pair* p=calced_bodys[arghash];
    Frame& new_env=child_frame[arghash];
    exprProcedure.reverse_proc_expr_group(p,new_env,pass_val ,std::bind(&ExprInterpretor::reverse_diff_expr, &exprProcedure, _1, _2, _3));
}

void ExprInterpretor::reverse_diff_expr(Pair *root, Frame &env, CalcRes pass_val) {
    if(root == nullptr){ throw InterpretorErrorException(); }
    if(root->is_number()){
        //    //case 1. number
        return ;
    }else if(root->is_symbol()){
        //case 2. symbol
        string symbol=root->getSymbol();
        if(reverse_diff_symbol_set.count(symbol)){
            if(reverse_diff_res.count(symbol)!=0){
                reverse_diff_res[symbol]=reverse_diff_res[symbol].element_wise_op(pass_val,std::plus<double>());
            }else{
                reverse_diff_res[symbol]=pass_val;
            }
            return;
        }
        Procedure&  proc=env.lookup(root->getSymbol());
        proc.reverse_diff_call(*this,pass_val,env,nullptr);
        return;
    }else{
        //case 3. P(symbol,P(arg1,P(arg2,...(root指向第一个P)
        Pair* first= root->first; Pair* second= root->second;
        if(first->is_symbol()){
            if(first->getSymbol()=="define"){
                return;
            }else{
                Procedure&  proc=env.lookup(first->getSymbol());
                proc.reverse_diff_call(*this,pass_val,env,second);
                return;
            }
        }
    }
    throw InterpretorErrorException();
}


void Frame::define(string symbol, shared_ptr<Procedure> value) {
    bindings[symbol]=value;
}

Procedure& Frame::lookup(string symbol) {
    if(bindings.count(symbol)!=0){
        return *bindings[symbol];
    }else if(parent){
        return parent->lookup(symbol);
    }
    throw InterpretorErrorException("Unknown identifier "+ symbol);
}

FuncCallProcedure::FuncCallProcedure(Pair *procedure_body, Pair* procedure_args, ExprInterpretor& exprInterpretor)
: uncalced_body(procedure_body), procedure_args(procedure_args),exprProcedure(exprInterpretor) { }

string FuncCallProcedure::arg_hash(Pair *args) {
    string s="";
    while(args){
        s+=(","+ args->first->get_forward_calcres().showRes());
        args=args->second;
    }
    return s;
}

FuncCallProcedure::~FuncCallProcedure() {
    for(pair<string,Pair*> pair:calced_bodys){ pair.second->release(); }
}

Frame &FuncCallProcedure::make_child_frame(Frame& env,Pair *args) {
    Pair* formals=procedure_args;
    child_frame[arg_hash(args)]= Frame(&env);
    Frame& frame=child_frame[arg_hash(args)];
    if(args==nullptr) return frame;
    if(args->len()!=formals->len()){ throw InterpretorErrorException("args number error"); }
    while(formals){
        if(args->first->is_number()||args->first->is_symbol()){
            frame.define(formals->first->getSymbol(),make_shared<ExprProcedure>(args->first,env,exprProcedure));
        }else if(args->first->is_node()){
            // func pos(x,y):
            //      x+y
            // pos(1+2,1+2+3) 函数参数是表达式时,先计算函数
            frame.define(formals->first->getSymbol(),make_shared<ExprProcedure>(args->first,env,exprProcedure));
        }
        formals=formals->second;
        args=args->second;
    }
    return frame;
}

ExprProcedure::ExprProcedure(Pair *procedure_body,Frame& env, ExprInterpretor &expr):body(procedure_body),env(env),exprInterpretor(expr) { }

CalcRes ExprProcedure::forward_calc_call(Frame& env,Pair *args) {
    return exprInterpretor.forward_calc_expr(body,this->env);
}

CalcRes ExprProcedure::forward_diff_call(Frame& env,Pair *args) {
    return exprInterpretor.forward_diff_expr(body,this->env);
}

void ExprProcedure::reverse_diff_call(ExprInterpretor &exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {
    exprProcedure.reverse_diff_expr(body,this->env,pass_val);
}
