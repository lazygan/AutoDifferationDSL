//
// Created by gangan on 2021/5/25.
//

#ifndef DSL_INTERPRETOR_H
#define DSL_INTERPRETOR_H

#include "dsl_utils.h"
#include "DSL/InterpretorImplements/InterpretorImplements.h"
#include "DSL/InterpretorImplements/PrimitiveProcedure.h"
#include <set>
#include <vector>
using namespace  std;
class Interpretor {
private:
    const AST & ast_tree;
    Frame global_frame;
    FuncCallProcedure main_program;
    ExprInterpretor exprProcedure;
public:
    Interpretor(const AST& ast);
    CalcRes forward_calc();
    CalcRes forward_diff(string variables);
    map<string,CalcRes> reverse_diff(vector<string> variables);
    map<string,CalcRes> forward_diff(vector<string> variables);
};


#endif //DSL_INTERPRETOR_H
