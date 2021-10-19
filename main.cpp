#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "DSL/FrontEnd/Lexer.h"
#include "DSL/FrontEnd/Parser.h"
#include "DSL/Interpretor.h"
using namespace std;


int main()
{

    string file_name("../../patch.gan");

    Lexer lexer(file_name);
    list<Token> tokens;
    try{
        tokens=lexer.tokenize();
    } catch (SyntaxErrorException e) {
        cout<<e.what()<<endl;
        exit(0);
    }
    TokenStream tokenStream(tokens);
    Parser parser(tokenStream);
    AST ast(nullptr);
    try{
        ast=parser.parse();
        print_pair(ast.root);
    } catch (SyntaxErrorException e) {
        cout<<e.what()<<endl;
        exit(0);
    }
    try{
        Interpretor interpretor=Interpretor(ast);
        cout<<"========================================="<<endl;
        cout<<"Forward Calc Result:"<<endl;
        cout<<interpretor.forward_calc().showRes()<<endl;
        cout<<"========================================="<<endl;
        cout<<"Forward Diff Result:"<<endl;
        cout<<"L:    "<<interpretor.forward_diff("L").showRes()<<endl;
        cout<<"W:    "<<interpretor.forward_diff("W").showRes()<<endl;
        cout<<"h:    "<<interpretor.forward_diff("h").showRes()<<endl;
        cout<<"========================================="<<endl;
        cout<<"Reverse Diff Result:"<<endl;
        map<string,CalcRes> res_map=interpretor.reverse_diff({"L","W","h"});
        for(auto iter=res_map.begin();iter!=res_map.end();iter++){
            cout<<iter->first<<":    "<<iter->second.showRes()<<endl;
        }
    } catch (InterpretorErrorException e) {
        cout<<e.what()<<endl;
        exit(0);
    }
}