//
// Created by gangan on 2021/5/27.
//

#ifndef DSL_PRIMITIVEPROCEDURE_H
#define DSL_PRIMITIVEPROCEDURE_H
#include "../dsl_utils.h"
#include "InterpretorImplements.h"


class PlusPrimitiveProcedure : public Procedure{
public:
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};

class MulPrimitiveProcedure : public  Procedure{
public:
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};

class SubPrimitiveProcedure : public  Procedure{
public:
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};

class DivPrimitiveProcedure: public  Procedure{
public:
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};

class SqrtPrimitiveProcedure: public  Procedure{
public:
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};

class SinPrimitiveProcedure: public  Procedure {
public:
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};

class CosPrimitiveProcedure: public  Procedure {
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};




class ListPrimitiveProcedure: public  Procedure{
public:
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};


class SubscriptPrimitiveProcedure: public  Procedure{
public:
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};






class ComparePrimitiveProcedure: public Procedure{
protected:
    virtual double cmp_proc(double,double) = 0;
public:
    CalcRes forward_calc_call(Frame& env,Pair* args= nullptr) override ;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};


class LessPrimitiveProcedure:public ComparePrimitiveProcedure{ double cmp_proc(double a,double b) override ; };

class LeqPrimitiveProcedure:public ComparePrimitiveProcedure{ double cmp_proc(double a,double b) override ; };

class GreatPrimitiveProcedure:public ComparePrimitiveProcedure{ double cmp_proc(double a,double b) override ; };

class GeqPrimitiveProcedure:public ComparePrimitiveProcedure{ double cmp_proc(double a,double b) override ; };

class EqPrimitiveProcedure:public ComparePrimitiveProcedure{ double cmp_proc(double a,double b) override ; };


class IfProcedure:public Procedure{
public:
    CalcRes forward_calc_call(Frame& env,Pair* p= nullptr) override;
    CalcRes forward_diff_call(Frame& env,Pair* args= nullptr) override ;
    void reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair* args= nullptr) override;
};


#endif //DSL_PRIMITIVEPROCEDURE_H
