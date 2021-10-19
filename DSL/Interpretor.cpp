//
// Created by gangan on 2021/5/25.
//

#include "Interpretor.h"
#include <memory>
using namespace std;
Interpretor::Interpretor(const AST &ast):ast_tree(ast),main_program(ast_tree.root,nullptr,exprProcedure) {
    global_frame.define("+" ,make_shared<PlusPrimitiveProcedure>());
    global_frame.define("-" ,make_shared<SubPrimitiveProcedure>());
    global_frame.define("*" ,make_shared<MulPrimitiveProcedure>());
    global_frame.define("/" ,make_shared<DivPrimitiveProcedure>());
    global_frame.define("sqrt" ,make_shared<SqrtPrimitiveProcedure>());
    global_frame.define("list" ,make_shared<ListPrimitiveProcedure>());
    global_frame.define("sin" ,make_shared<SinPrimitiveProcedure>());
    global_frame.define("cos" ,make_shared<CosPrimitiveProcedure>());
    global_frame.define("<" ,make_shared<LessPrimitiveProcedure>());
    global_frame.define("<=" ,make_shared<LeqPrimitiveProcedure>());
    global_frame.define(">" ,make_shared<GreatPrimitiveProcedure>());
    global_frame.define(">=" ,make_shared<GeqPrimitiveProcedure>());
    global_frame.define("==" ,make_shared<EqPrimitiveProcedure>());
    global_frame.define("if" ,make_shared<IfProcedure>());
    global_frame.define("subscript" ,make_shared<SubscriptPrimitiveProcedure>());
}



CalcRes Interpretor::forward_calc(){
    return main_program.forward_calc_call(global_frame);
}

map<string, CalcRes> Interpretor::forward_diff(vector<string> variables) {
    //先前向计算
    map<string,CalcRes> res;
    for(string& var:variables){
        res[var]=forward_diff(var);
    }
    return res;
}

CalcRes Interpretor::forward_diff(string symbol) {
    main_program.exprProcedure.forward_diff_symbol=symbol;
    main_program.forward_calc_call(global_frame);
    return main_program.forward_diff_call(global_frame);
}

map<string, CalcRes> Interpretor::reverse_diff(vector<string> variables) {
    set<string>& symbol_set=exprProcedure.reverse_diff_symbol_set;
    for(string& var:variables) { symbol_set.insert(var); }

    CalcRes res=main_program.forward_calc_call(global_frame);
    vector<CalcRes> reverse_enumerate = res.reverse_input_enumerate();
    map<string,CalcRes> resMap;
    for(CalcRes& cr: reverse_enumerate){
        exprProcedure.reverse_diff_res.clear();
        main_program.reverse_diff_call(exprProcedure,cr,global_frame);
        for(auto iter=exprProcedure.reverse_diff_res.begin();iter!=exprProcedure.reverse_diff_res.end();iter++){
            CalcRes val=cr.element_wise_op(cr.fill(iter->second.get_number()),std::multiplies<double>());
            if(resMap.count(iter->first)!=0){
                resMap[iter->first] = resMap[iter->first].element_wise_op(val,std::plus<double>());
            }else{
                resMap[iter->first] = val;
            }
        }
    }
    return resMap;
}



