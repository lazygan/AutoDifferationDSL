//
// Created by gangan on 2021/5/21.
//

#ifndef DSL_DSL_UTILS_H
#define DSL_DSL_UTILS_H
#include <string>
#include <memory>
#include <iostream>
#include <functional>
#include <vector>
using namespace std;
class SyntaxErrorException{
private:
    const string message;
public:
    SyntaxErrorException(const string& str = "Syntax Error"):message(str) { }
    SyntaxErrorException(int line_num,int column_num ,const string& str = "Syntax Error")
    :message(str+". "+to_string(line_num)+","+to_string(column_num)) {}
    string what() const { return message; }
};

class InterpretorErrorException {
private:
    const string message;
public:
    InterpretorErrorException(const string &str = "Interptretor Error") : message(str) {}
    string what() const { return message; }
};

struct CalcResNode{
    double val=DBL_MAX;
    CalcResNode* first=nullptr;
    CalcResNode* second=nullptr;
    CalcResNode()=default;
    CalcResNode(double val):val(val){}
};

class CalcRes{
private:
    CalcResNode* copy(CalcResNode* root) const;
    void post_order_visit(CalcResNode* n,function<void(CalcResNode*)> proc) const;
    void post_order_visit_(CalcResNode* res,CalcResNode* b,function<double(double,double)> proc) const;
    string showRes(CalcResNode* n);
    bool isSameTopo(const CalcResNode* a, const CalcResNode* b) const;
    CalcRes(CalcResNode* root):root(root){};
    void recur_reverse_input_enumerate(CalcResNode* root,CalcResNode* iter,vector<CalcRes>& res);
public:
    CalcResNode* root=nullptr;
    CalcRes()=default;
    CalcRes(double val);
    CalcRes(CalcRes&& c);
    CalcRes(const CalcRes& c);
    CalcRes& operator = (const CalcRes& c);
    CalcRes& operator = (CalcRes&& c);
    ~CalcRes();
    bool is_empty();
    bool is_number();
    double get_number();
    CalcRes expend_dim();
    CalcRes fill(double val);
    CalcRes push_back(const CalcRes& c);
    CalcRes element_wise_op(const CalcRes& otherCalcRes,function<double (double,double)> op) const;
    CalcRes element_wise_op(function<double (double)> op) const ;
    CalcRes subscirpt(vector<int> index);
    CalcRes reverse_subsecript(vector<int> index,const CalcRes& sub_res);
    string showRes();
    vector<CalcRes> reverse_input_enumerate();
};



class Pair{
private:
    bool is_calced_flag=false;
    bool is_diff_flag=false;
    CalcRes forward_calcres;
    CalcRes forward_diffres;
    double val=DBL_MAX;
    string val_str="";
    void travel(function<void(Pair*)> func);
public:
    Pair()=default;
    Pair(Pair* fst,Pair* sec): first(fst), second(sec){ }
    Pair(string val):val_str(val){ }
    Pair(double val):val(val){ }
    Pair(CalcRes cr): forward_calcres(cr), is_calced_flag(true){ }
    Pair* first=nullptr;
    Pair* second=nullptr;
    bool is_symbol();
    bool is_number();
    bool is_node();
    bool is_calced();
    double getNumber();
    string& getSymbol();

    CalcRes& set_forward_calcres(const CalcRes& val);
    CalcRes& get_forward_calcres();

    CalcRes& set_forward_diffres(const CalcRes& val);
    CalcRes& get_forward_diffres();

    int len();
    void release();
    Pair* clone_pair();
};

struct AST{
    Pair* root;
    AST(Pair* root):root(root){}
    AST(const AST& ast);
    AST(AST&& ast);
    AST& operator =(const AST& ast);
    AST& operator =(AST&& ast);
    ~AST();
};


string print_pair(Pair* p);

#endif //DSL_DSL_UTILS_H
