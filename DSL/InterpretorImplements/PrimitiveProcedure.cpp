//
// Created by gangan on 2021/5/27.
//

#include "PrimitiveProcedure.h"
#include "math.h"
struct Pow {
    double operator()(double a, double b) {
        return std::pow(a,b);
    }
};
struct Sin{
    double operator()(double a) {
        return std::sin(a);
    }
};

struct Cos{
    double operator()(double a) {
        return std::cos(a);
    }
};

struct Tan{
    double operator()(double a) {
        return std::tan(a);
    }
};

struct Sec{
    double operator()(double a) {
        return 1.0/std::cos(a);
    }
};


map<string,function<double(double,double)>> binary_op=
        {
                {"+",std::plus<double>()},
                {"-",std::minus<double>()},
                {"*",std::multiplies<double>()},
                {"/",std::divides<double>()},
                {"**",Pow()}
        };

map<string,function<double(double)>> unary_op=
        {
                {"-",std::negate<double>()},
                {"sin",Sin()},
                {"cos",Cos()},
                {"tan",Tan()},
                {"sec",Sec()},
        };

CalcRes expr(const CalcRes& a,string op,const CalcRes& b){
    return a.element_wise_op(b,binary_op[op]);
}
CalcRes expr(string op,const CalcRes& a){
    return a.element_wise_op(unary_op[op]);
}


CalcRes PlusPrimitiveProcedure::forward_calc_call(Frame& env,Pair *args) {
    if(args->len()!=2){ throw InterpretorErrorException( typeid(*this).name() ); }
    CalcRes left =args->first->get_forward_calcres();
    CalcRes right=args->second->first->get_forward_calcres();
    return expr(left,"+",right);
}

CalcRes PlusPrimitiveProcedure::forward_diff_call(Frame& env,Pair *args) {
    CalcRes left =args->first->get_forward_diffres();
    CalcRes right=args->second->first->get_forward_diffres();
    return expr(left,"+",right);
}

void PlusPrimitiveProcedure::reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair *args) {
    if(args->len()!=2){ throw InterpretorErrorException(typeid(*this).name()); }
    exprProcedure.reverse_diff_expr(args->first,env,pass_val);
    exprProcedure.reverse_diff_expr(args->second->first,env,pass_val);

}

CalcRes MulPrimitiveProcedure::forward_calc_call(Frame& env,Pair *args) {
    if(args->len()!=2){ throw InterpretorErrorException(typeid(*this).name()); }
    CalcRes left =args->first->get_forward_calcres();
    CalcRes right=args->second->first->get_forward_calcres();
    return expr(left,"*",right);
}

CalcRes MulPrimitiveProcedure::forward_diff_call(Frame& env,Pair *args) {
    if(args->len()!=2){ throw InterpretorErrorException(typeid(*this).name()); }
    CalcRes lc=args->first->get_forward_calcres();
    CalcRes rc=args->second->first->get_forward_calcres();
    CalcRes ld=args->first->get_forward_diffres();
    CalcRes rd=args->second->first->get_forward_diffres();
    return expr(expr(ld,"*",rc),"+",expr(lc,"*",rd));
}

void MulPrimitiveProcedure::reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame& env, Pair *args) {
    CalcRes lc=args->first->get_forward_calcres();
    CalcRes rc=args->second->first->get_forward_calcres();

    exprProcedure.reverse_diff_expr(args->first,env,expr(pass_val,"*",rc));
    exprProcedure.reverse_diff_expr(args->second->first,env,expr(pass_val,"*",lc));

}

CalcRes SubPrimitiveProcedure::forward_calc_call(Frame& env,Pair *args) {
    if(args->len()==2){
        CalcRes lc=args->first->get_forward_calcres();
        CalcRes rc=args->second->first->get_forward_calcres();
        return expr(lc,"-",rc);
    }else if(args->len()==1){
        CalcRes lc=args->first->get_forward_calcres();
        return  expr("-",lc);
    }
    throw InterpretorErrorException(typeid(*this).name());
}

CalcRes SubPrimitiveProcedure::forward_diff_call(Frame& env,Pair *args) {
    if(args->len()==2){
        CalcRes ld=args->first->get_forward_diffres();
        CalcRes rd=args->second->first->get_forward_diffres();
        return expr(ld,"-",rd);
    }else if(args->len()==1){
        CalcRes ld=args->first->get_forward_diffres();
        return  expr("-",ld);
    }
    throw InterpretorErrorException(typeid(*this).name());
}

void SubPrimitiveProcedure::reverse_diff_call(ExprInterpretor &exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {
    if(args->len()==2){
        exprProcedure.reverse_diff_expr(args->first,env,pass_val);
        exprProcedure.reverse_diff_expr(args->second->first,env,expr("-",pass_val));
    }else if(args->len()==1) {
        exprProcedure.reverse_diff_expr(args->first,env,expr("-",pass_val));
    }
}

CalcRes DivPrimitiveProcedure::forward_calc_call(Frame& env,Pair *args) {
    if(args->len()!=2){ throw InterpretorErrorException(typeid(*this).name()); }
     CalcRes lc=args->first->get_forward_calcres();
     CalcRes rc=args->second->first->get_forward_calcres();
    return expr(lc,"/",rc);
}

CalcRes DivPrimitiveProcedure::forward_diff_call(Frame& env,Pair *args) {
    CalcRes nc=args->first->get_forward_calcres();
    CalcRes dc=args->second->first->get_forward_calcres();
    CalcRes nd=args->first->get_forward_diffres();
    CalcRes dd=args->second->first->get_forward_diffres();

    CalcRes numerator=expr(expr(nd,"*",dc) ,"-",expr(dd,"*",nc));
    CalcRes denominator=expr(dc,"*",dc);

    return expr(numerator,"/",denominator);
}

void DivPrimitiveProcedure::reverse_diff_call(ExprInterpretor &exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {
    CalcRes nc=args->first->get_forward_calcres();
    CalcRes dc=args->second->first->get_forward_calcres();
    exprProcedure.reverse_diff_expr(args->first,env,expr(pass_val,"/",dc));
    exprProcedure.reverse_diff_expr(args->second->first,env,expr("-", expr(expr(pass_val,"*",nc) ,"/",expr(dc,"*",dc))));
}

CalcRes SqrtPrimitiveProcedure::forward_calc_call(Frame& env,Pair *args) {
    if(args->len()!=1){ throw InterpretorErrorException(typeid(*this).name()); }
    CalcRes c=args->first->get_forward_calcres();
    CalcRes cons=c.fill(0.5);
    return expr(c,"**",cons);
}

CalcRes SqrtPrimitiveProcedure::forward_diff_call(Frame& env,Pair *args) {
    CalcRes c=args->first->get_forward_calcres();
    CalcRes d=args->first->get_forward_diffres();
    CalcRes cons=c.fill(0.5);
    return expr(expr(cons,"*",expr(c,"**",expr("-",cons))),"*",d);
}

void SqrtPrimitiveProcedure::reverse_diff_call(ExprInterpretor &exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {
    CalcRes c=args->first->get_forward_calcres();
    CalcRes cons=c.fill(0.5);
    exprProcedure.reverse_diff_expr(args->first,env,expr(pass_val,"*",expr(cons,"*",expr(c,"**",expr("-",cons)))));
}


CalcRes SinPrimitiveProcedure::forward_calc_call(Frame& env,Pair *args) {
    if(args->len()!=1){ throw InterpretorErrorException(typeid(*this).name()); }
    CalcRes c=args->first->get_forward_calcres();
    return expr("sin",c);
}

CalcRes SinPrimitiveProcedure::forward_diff_call(Frame &env, Pair *args) {
    CalcRes c=args->first->get_forward_calcres();
    CalcRes d=args->first->get_forward_diffres();
    return expr(expr("cos",c),"*",d);
}

void SinPrimitiveProcedure::reverse_diff_call(ExprInterpretor &exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {
    CalcRes c=args->first->get_forward_calcres();
    exprProcedure.reverse_diff_expr(args->first,env,expr(pass_val,"*",expr("cos",c)));
}

CalcRes CosPrimitiveProcedure::forward_calc_call(Frame &env, Pair *args) {
    if(args->len()!=1){ throw InterpretorErrorException(typeid(*this).name()); }
    CalcRes c=args->first->get_forward_calcres();
    return expr("cos",c);
}

CalcRes CosPrimitiveProcedure::forward_diff_call(Frame &env, Pair *args) {
    CalcRes c=args->first->get_forward_calcres();
    CalcRes d=args->first->get_forward_diffres();
    return expr(expr("-",expr("sin",c)),"*",d);
}

void CosPrimitiveProcedure::reverse_diff_call(ExprInterpretor &exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {
    CalcRes c=args->first->get_forward_calcres();
    exprProcedure.reverse_diff_expr(args->first,env,expr(pass_val,"*",expr("-",expr("sin",c))));
}



CalcRes ListPrimitiveProcedure::forward_calc_call(Frame& env,Pair *args) {
    if(args==nullptr) throw InterpretorErrorException();
    CalcRes listRes(args->first->get_forward_calcres().expend_dim());
    args=args->second;
    while(args){
        listRes=listRes.push_back(args->first->get_forward_calcres().expend_dim());
        args=args->second;
    }
    return listRes;
}

CalcRes ListPrimitiveProcedure::forward_diff_call(Frame& env,Pair *args) {
    if(args==nullptr) throw InterpretorErrorException();
    CalcRes listRes(args->first->get_forward_diffres().expend_dim());
    args=args->second;
    while(args){
        listRes=listRes.push_back(args->first->get_forward_diffres().expend_dim());
        args=args->second;
    }
    return listRes;
}

void ListPrimitiveProcedure::reverse_diff_call(ExprInterpretor& exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {
    if(args==nullptr) throw InterpretorErrorException();
    int i=0;
    while(args){
        CalcRes pv=pass_val.subscirpt({i});
        exprProcedure.reverse_diff_expr(args->first,env,pv);
        args=args->second;
        i++;
    }
}

CalcRes ComparePrimitiveProcedure::forward_calc_call(Frame& env,Pair *args) {
    if(args->len()!=2){ throw InterpretorErrorException(typeid(*this).name()); }
    CalcRes mask=args->first->get_forward_calcres().element_wise_op(args->second->first->get_forward_calcres(), std::bind(&ComparePrimitiveProcedure::cmp_proc, this, _1, _2));
    return mask;
}
CalcRes ComparePrimitiveProcedure::forward_diff_call(Frame& env,Pair *args) { return CalcRes(); }
void ComparePrimitiveProcedure::reverse_diff_call(ExprInterpretor &exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {}



CalcRes IfProcedure::forward_calc_call(Frame& env,Pair *args) {
    if (args->len() != 3) { throw InterpretorErrorException(typeid(*this).name()); }

    CalcRes mask = args->first->get_forward_calcres();
    CalcRes true_stmnt = args->second->first->get_forward_calcres();
    CalcRes false_stmnt = args->second->second->first->get_forward_calcres();
    if(mask.is_number() ){
            mask=mask.root->val!=0 ?true_stmnt.fill(1.0): true_stmnt.fill(0.0);
    }
    CalcRes res = true_stmnt.element_wise_op(mask, std::multiplies<double>());
    auto not_op = [](double a) -> double { return abs(a - 1.0); };
    res = res.element_wise_op( false_stmnt.element_wise_op( mask.element_wise_op(not_op), std::multiplies<double>() ), std::plus<double>() );
    return res;
}

CalcRes IfProcedure::forward_diff_call(Frame& env,Pair *args) {
    if (args->len() != 3) { throw InterpretorErrorException(typeid(*this).name()); }
    CalcRes mask = args->first->get_forward_calcres();
    CalcRes true_stmnt = args->second->first->get_forward_diffres();
    CalcRes false_stmnt = args->second->second->first->get_forward_diffres();
    if(mask.is_number() ){
        mask=mask.root->val!=0 ?true_stmnt.fill(1.0): true_stmnt.fill(0.0);
    }
    CalcRes res = true_stmnt.element_wise_op(mask, std::multiplies<double>());
    auto not_op = [](double a) -> double { return abs(a - 1.0); };
    res = res.element_wise_op( false_stmnt.element_wise_op( mask.element_wise_op(not_op), std::multiplies<double>() ), std::plus<double>() );
    return res;
}

void IfProcedure::reverse_diff_call(ExprInterpretor &exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {
    CalcRes mask = args->first->get_forward_calcres();
    if(mask.is_number() ) {
        mask = mask.root->val != 0 ? pass_val.fill(1.0) : pass_val.fill(0.0);
    }


        CalcRes pass_val_true_stmnt = pass_val.element_wise_op(mask,std::multiplies<double>());
    exprProcedure.reverse_diff_expr(args->second->first,env,pass_val_true_stmnt);
    auto not_op = [](double a) -> double { return abs(a - 1.0); };
    CalcRes pass_val_false_stmnt = pass_val.element_wise_op(mask.element_wise_op(not_op),std::multiplies<double>());
    exprProcedure.reverse_diff_expr(args->second->second->first,env,pass_val_false_stmnt);
}

double LessPrimitiveProcedure::cmp_proc(double a, double b) { return a<b?1.0:0.0; }

double LeqPrimitiveProcedure::cmp_proc(double a, double b) { return a<=b?1.0:0.0; }

double GeqPrimitiveProcedure::cmp_proc(double a, double b) { return a>=b?1.0:0.0; }

double GreatPrimitiveProcedure::cmp_proc(double a, double b) { return a>b?1.0:0.0; }

double EqPrimitiveProcedure::cmp_proc(double a, double b) { return a==b?1.0:0.0; }






CalcRes SubscriptPrimitiveProcedure::forward_calc_call(Frame& env,Pair *args) {
    if (args->len() < 2) { throw InterpretorErrorException(typeid(*this).name()); }
    vector<int> indexes;
    while(args->second!=nullptr){
        indexes.push_back(static_cast<int>(args->first->getNumber()));
        args=args->second;
    }
    return args->first->get_forward_calcres().subscirpt(indexes);
}

CalcRes SubscriptPrimitiveProcedure::forward_diff_call(Frame& env,Pair *args) {
    vector<int> indexes;
    while(args->second!=nullptr){
        indexes.push_back(static_cast<int>(args->first->getNumber()));
        args=args->second;
    }
    return args->first->get_forward_diffres().subscirpt(indexes);
}

void SubscriptPrimitiveProcedure::reverse_diff_call(ExprInterpretor &exprProcedure, CalcRes pass_val, Frame &env, Pair *args) {
    vector<int> indexes;
    while(args->second!=nullptr){
        indexes.push_back(static_cast<int>(args->first->getNumber()));
        args=args->second;
    }
    CalcRes mask=args->first->get_forward_calcres().fill(0.0);
    pass_val=mask.reverse_subsecript(indexes,pass_val);
    exprProcedure.reverse_diff_expr(args->first,env,pass_val);
}
