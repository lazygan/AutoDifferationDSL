//
// Created by gangan on 2021/5/21.
//

#include "dsl_utils.h"

string print_pair(Pair* p) {
    string s="";
    if(p==nullptr){ s+="nil"; return s; }
    if(p->is_number()){ s+=p->getNumber(); return s;}
    if(p->is_symbol()){ s+=p->getSymbol(); return s; }
    s+="P("+print_pair(p->first)+","+print_pair(p->second)+")";
    return s;
}

bool Pair::is_symbol() { return first==nullptr && second==nullptr && val==DBL_MAX && val_str!=""; }
bool Pair::is_number() { return first==nullptr && second==nullptr && val!=DBL_MAX && val_str==""; }
bool Pair::is_node() { return (first!=nullptr || second!=nullptr) && val==DBL_MAX && val_str==""; }

int Pair::len() {
    if(this==nullptr) return 0;
    if(second==nullptr){ return 1; }
    Pair* p=second;
    int count=1;
    while(p){
        count++;
        p=p->second;
    }
    return count;
}

CalcRes& Pair::set_forward_calcres(const CalcRes& val) {
    forward_calcres=val;
    is_calced_flag=true;
    return forward_calcres;
}

CalcRes& Pair::get_forward_calcres() {
    if(!is_calced_flag){
        throw InterpretorErrorException("Internal Node wasn't Calced");
    }
    return forward_calcres;
}

double Pair::getNumber() {
    if(!is_number()){
        throw InterpretorErrorException("Node wasn't Number Node");
    }
    return val;
}

string& Pair::getSymbol() {
    if(!is_symbol()){
        throw InterpretorErrorException("Node wasn't Symbol Node");
    }
    return val_str;
}

void Pair::travel(function<void(Pair *)> func) {
    if(this==nullptr) return ;
    if(second!=nullptr){ second->travel(func); }
    if(first!=nullptr){ first->travel(func); }
    func(this);
}

void Pair::release() {
    auto rel = [](Pair* p){ delete p; };
    travel(rel);
}

Pair *Pair::clone_pair() {
    Pair* root;
    root=new Pair(*this);
    if(first)
        root->first=first->clone_pair();
    if(second)
        root->second=second->clone_pair();
    return root;
}

CalcRes &Pair::set_forward_diffres(const CalcRes &val) {
    forward_diffres=val;
    is_calced_flag=true;
    return forward_diffres;
}

CalcRes &Pair::get_forward_diffres() {
    return forward_diffres;
}

bool Pair::is_calced() {
    return is_calced_flag;
}



CalcRes::CalcRes(const CalcRes& calcRes) {
    root=copy(calcRes.root);
}

CalcResNode* CalcRes::copy(CalcResNode* target) const{
    if(target==nullptr) return nullptr;
    CalcResNode* root=new CalcResNode(target->val);
    CalcResNode* n=root;
    if(target->first){
        n->first=copy(target->first);
    }
    while(target->second){
        n->second=new CalcResNode(target->second->val);
        target=target->second;
        n=n->second;
        if(target->first){
            n->first=copy(target->first);
        }
    }
    return root;
}

void CalcRes::post_order_visit(CalcResNode* n,function<void(CalcResNode *)> proc) const{
    if(n==nullptr) return ;
    post_order_visit(n->first,proc);
    post_order_visit(n->second,proc);
    proc(n);
}

CalcRes::~CalcRes() {
    auto delete_node=[](CalcResNode* n)->void{ delete n; };
    post_order_visit(root,delete_node);
}

string CalcRes::showRes(CalcResNode* n) {
    if(n==nullptr) return "";
    string res="";
    res+="[";
    while(n){
        if(n->first!=nullptr){
            res+=showRes(n->first);
            res+=",";
        }else{
            res+=to_string(n->val)+",";
        }
        n=n->second;
    }
    res+="]";
    return res;
}

string CalcRes::showRes() {
    return showRes(root);
}

CalcRes::CalcRes(double val) {
    root=new CalcResNode(val);
}

bool CalcRes::isSameTopo(const CalcResNode* a,const CalcResNode* b) const{
    while(a&&b){
        if(a->first!=nullptr && b->first!=nullptr){
            if(!isSameTopo(a->first,b->first)){
                return false;
            }
        }else if( a->first!=nullptr || b->first!=nullptr ){
            return false;
        }
        a=a->second;
        b=b->second;
    }
    if(a==nullptr&&b==nullptr){
        return true;
    }
    return false;
}

CalcRes CalcRes::element_wise_op(const CalcRes &otherCalcRes, function<double (double, double)> op) const{
    if(!isSameTopo(root,otherCalcRes.root)) throw InterpretorErrorException("Dim Error");
    CalcResNode* res_root=copy(root);
    post_order_visit_(res_root,otherCalcRes.root,op);
    return CalcRes(res_root);
}

void CalcRes::post_order_visit_(CalcResNode* res,CalcResNode* b,function<double(double,double)> proc) const{
    if(res==nullptr) return;
    post_order_visit_(res->first,b->first,proc);
    post_order_visit_(res->second,b->second,proc);
    res->val=proc(res->val,b->val);
}

CalcRes CalcRes::element_wise_op(function<double(double)> op) const{
    CalcResNode* res_root=copy(root);
    auto proc=[&](CalcResNode* n)->void{ n->val=op(n->val); };
    post_order_visit(res_root,proc);
    return res_root;
}

CalcRes::CalcRes(CalcRes &&c) {
    root=c.root; c.root=nullptr;
}


CalcRes& CalcRes::operator=(const CalcRes &c) {
    if(this==&c ) return *this;
    root=copy(c.root);
    return *this;
}

CalcRes CalcRes::push_back(const CalcRes &c) {
    if(root==nullptr) throw InterpretorErrorException();
    CalcResNode* new_root=copy(root);
    CalcResNode* other_root=copy(c.root);
    CalcResNode* tail=new_root;
    while(tail->second){ tail=tail->second; }
    tail->second=other_root;
    return CalcRes(new_root);
}

CalcRes CalcRes::expend_dim() {
    CalcResNode* new_root=new CalcResNode();
    new_root->first=copy(root);
    return CalcRes(new_root);
}

CalcRes &CalcRes::operator=(CalcRes&& c) {
    if(this==&c ) return *this;
    root=c.root; c.root=nullptr;
    return *this;
}

bool CalcRes::is_empty() {
    return root==nullptr;
}

CalcRes CalcRes::fill(double val) {
    CalcResNode* new_root=new CalcResNode();
    new_root=copy(root);
    auto fill_proc=[&val](CalcResNode* p)->void{
        p->val=val;
    };
    post_order_visit(new_root,fill_proc);
    return CalcRes(new_root);
}

CalcRes CalcRes::subscirpt(vector<int> index) {
    CalcResNode* p=root;
    for(int ind:index){
        for(int i=0;i<ind;i++){
            p=p->second;
            if(p==nullptr){
                throw InterpretorErrorException("subscript error");
            }
        }
        if(p->first==nullptr){
            throw InterpretorErrorException("subscript error");
        }
        p=p->first;
    }
    CalcResNode* new_root=copy(p);
    return CalcRes(new_root);
}

CalcRes CalcRes::reverse_subsecript(vector<int> index, const CalcRes &sub_res) {
    CalcResNode* new_root=copy(root);
    CalcResNode* p=new_root;
    for(int ind:index){
        for(int i=0;i<ind;i++){
            p=p->second;
        }
        if(p->first==nullptr){
            throw InterpretorErrorException("subscript error");
        }
        p=p->first;
    }
    if(!isSameTopo(p,sub_res.root)){ throw InterpretorErrorException("reverse subscript error"); }
    auto copy =[](double a,double b)->double{ return b; };
    post_order_visit_(p,sub_res.root,copy);
    return new_root;
}

bool CalcRes::is_number() {
    return root->first==nullptr &&  root->second==nullptr;
}

vector<CalcRes> CalcRes::reverse_input_enumerate() {
    CalcRes zero_calcres=fill(0);
    vector<CalcRes> res;
    recur_reverse_input_enumerate(zero_calcres.root,zero_calcres.root,res);
    return res;
}

void  CalcRes::recur_reverse_input_enumerate(CalcResNode *root,CalcResNode* iter,vector<CalcRes>& res) {
    if(iter==nullptr ) return;
    while(iter){
        if(iter->first!=nullptr){
            recur_reverse_input_enumerate(root,iter->first,res);
        }else{
            iter->val=1;
            CalcResNode* new_root=copy(root);
            res.push_back(CalcRes(new_root));
            iter->val=0;
        }
        iter=iter->second;
    }
}

double CalcRes::get_number() {
    if(!is_number()) throw InterpretorErrorException("CalcRes Not Number");
    return root->val;
}

AST::~AST() {
    root->release();
}

AST::AST(const AST &ast) {
    root=ast.root->clone_pair();
}

AST::AST(AST &&ast) {
    root=ast.root; ast.root=nullptr;
}

AST &AST::operator=(const AST& ast) {
    if(this==&ast) return *this;
    root=ast.root->clone_pair();
    return *this;
}

AST &AST::operator=(AST&& ast) {
    if(this==&ast) return *this;
    root=ast.root; ast.root=nullptr;
    return *this;
}
