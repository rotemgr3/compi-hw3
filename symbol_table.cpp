#include "symbol_table.h"
#include "hw3_output.hpp"
#include <algorithm>


extern int yylineno;

void SymbolTable::push_symbol(string type, string name, int offset){
    verify_new_symbol(name);
    shared_ptr<Symbol> new_symbol = make_shared<Symbol>(type, name, offset);
    symbols.push_back(new_symbol);
}

void SymbolTable::push_function_symbol(shared_ptr<FunctionSymbol> new_symbol){
    symbols.push_back(new_symbol);
}

void SymbolTable::verify_new_symbol(string name){
    for (auto symbol : symbols){
        if (symbol->name == name){
            output::errorDef(yylineno, name);
            exit(0);
        }
    }
}

vector<shared_ptr<Symbol>>::iterator SymbolTable::verify_new_function_symbol(shared_ptr<FunctionSymbol> new_symbol){
    if (new_symbol->name == "main" && new_symbol->is_override == true){
        output::errorMainOverride(yylineno);
        exit(0);
    }
    for (auto it=symbols.begin(); it!=symbols.end(); it++){
        if ((*it)->name != new_symbol->name){
            continue;
        }
        if ((*it)->type != "function"){
            continue;
        }
        shared_ptr<FunctionSymbol> function_symbol = dynamic_pointer_cast<FunctionSymbol>((*it));
        if (new_symbol->is_override == false && function_symbol->is_override == true){
            output::errorOverrideWithoutDeclaration(yylineno, new_symbol->name);
            exit(0);
        }
        if (new_symbol->is_override == true && function_symbol->is_override == false){
            output::errorFuncNoOverride(yylineno, new_symbol->name);
            exit(0);
        }
        if (new_symbol->is_override == true && function_symbol->is_override == true) {
            if (new_symbol->ret_type->type != function_symbol->ret_type->type){
                return it;
            }
            if (new_symbol->args.size() != function_symbol->args.size()){
                return it;
            }
            for(int i = 0; i<new_symbol->args.size(); i++){
                if(new_symbol->args[i]->type != function_symbol->args[i]->type){
                    return it;
                }
            }  
        }
        if (new_symbol->is_override == false && function_symbol->is_override == false) {
            output::errorDef(yylineno, function_symbol->name);
            exit(0);
        }
    }
    return symbols.end();
}

shared_ptr<Symbol> SymbolTable::get_symbol(string name){
    for (auto symbol : symbols){
        if (symbol->name == name){
            return symbol;
        }
    }
    return nullptr;
}

SymbolTableStack::SymbolTableStack() : symbol_tables(), offsets() {
    symbol_tables.push_back(make_shared<SymbolTable>());
    offsets.push_back(0);

    shared_ptr<Formals> formals_print = make_shared<Formals>(make_shared<Formalslist>(make_shared<Formaldecl>(make_shared<Type>("string"), make_shared<Node>("string"))));
    shared_ptr<Funcdecl> func_decl_print =  make_shared<Funcdecl>(make_shared<Override>(false), make_shared<Rettype>(), make_shared<Node>("print"), formals_print);

    shared_ptr<Formals> formals_printi = make_shared<Formals>(make_shared<Formalslist>(make_shared<Formaldecl>(make_shared<Type>("int"), make_shared<Node>("int"))));
    shared_ptr<Funcdecl> func_decl_printi =  make_shared<Funcdecl>(make_shared<Override>(false), make_shared<Rettype>(), make_shared<Node>("printi"), formals_printi);
};


void SymbolTableStack::push_symbol_table(bool is_loop, string return_type){
    shared_ptr<SymbolTable> new_symbol_table = make_shared<SymbolTable>(is_loop, return_type);
    symbol_tables.push_back(new_symbol_table);
    if (offsets.empty()){
        offsets.push_back(0);
    }
    else{
        offsets.push_back(offsets.back());
    }
}

string upper(const string &str) {
    if (str == "bool")
        return "BOOL";
    else if (str == "byte")
        return "BYTE";
    else if (str == "int")
        return "INT";
    else if (str == "void")
        return "VOID";
    else
        return "STRING";
}

void SymbolTableStack::pop_symbol_table(){
    shared_ptr<SymbolTable> symbols = symbol_tables.back();
    symbol_tables.pop_back();
    offsets.pop_back();
    output::endScope();
    for (auto symbol : symbols->symbols){
        if (symbol->type == "function") {
            shared_ptr<FunctionSymbol> function_symbol = dynamic_pointer_cast<FunctionSymbol>(symbol);
            vector<string> args_types;
            for (auto arg : function_symbol->args) {
                args_types.push_back(upper(arg->type->type));
            }
            output::printID(symbol->name, symbol->offset, output::makeFunctionType(upper(function_symbol->ret_type->type->type), args_types));
        }
        else {
            output::printID(symbol->name, symbol->offset, upper(symbol->type));
        }
    }
}

void SymbolTableStack::push_symbol(string type, string name){
    int offset = offsets.back();
    symbol_tables.back()->push_symbol(type, name, offset);
    offsets.back()++;
    return;
    
}

void SymbolTableStack::push_function_symbol(shared_ptr<Funcdecl> funcdecl, bool is_decleration_only) {
    if (funcdecl->id == "main" && funcdecl->ret_type->type->type == "void" && funcdecl->formals.size() == 0){
        found_main = true;
    }
    shared_ptr<FunctionSymbol> new_symbol = make_shared<FunctionSymbol>("function", funcdecl->id, funcdecl->formals, funcdecl->is_override, funcdecl->ret_type);
    auto old_symbol_it = verify_new_function_symbol(new_symbol);
    symbol_tables.back()->push_function_symbol(new_symbol);

    if (is_decleration_only == false){
        push_symbol_table(false, funcdecl->ret_type->type->type);
        int offset = -1;
        for (auto it = funcdecl->formals.begin(); it != funcdecl->formals.end(); ++it) { 
            symbol_tables.back()->push_symbol((*it)->type->type, (*it)->id, offset);
            offset--;
        }
    }
}

void SymbolTableStack::verify_main(){
    if (found_main == true){
        return;
    }
    output::errorMainMissing();
    exit(0);
}
vector<shared_ptr<Symbol>>::iterator SymbolTableStack::verify_new_function_symbol(shared_ptr<FunctionSymbol> new_symbol) {
    if (symbol_tables.empty()) {
        return vector<shared_ptr<Symbol>>::iterator();
    }
    return symbol_tables[0]->verify_new_function_symbol(new_symbol);
}

shared_ptr<Symbol> SymbolTableStack::get_symbol(string name){
    for (auto symbol_table : symbol_tables){
        shared_ptr<Symbol> symbol = symbol_table->get_symbol(name);
        if (symbol != nullptr){
            return symbol;
        }
    }
    return nullptr;
}

void SymbolTableStack::verify_symbol(string name){
    auto symbol = get_symbol(name);
    if (symbol == nullptr){
        output::errorUndef(yylineno, name);
        exit(0);
    }
}

shared_ptr<SymbolTable> SymbolTableStack::get_current_symbol_table(){
    return symbol_tables.back();
}

shared_ptr<FunctionSymbol> SymbolTableStack::match_function_symbol(string name, vector<shared_ptr<Exp>> args) {
    if (symbol_tables.empty()) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
    shared_ptr<FunctionSymbol> res_func;
    auto first_symbol_table = symbol_tables[0];
    bool found = false;
    int match_count = 0;
    for (auto symbol : first_symbol_table->symbols) {
        if (symbol->name != name) {
            continue;
        }
        if (symbol->type != "function") {
            continue;
        }
        shared_ptr<FunctionSymbol> function_symbol = dynamic_pointer_cast<FunctionSymbol>(symbol);
        found = true;
        if (function_symbol->args.size() != args.size()) {
            continue;
        }
        int i;
        for (i = 0; i < args.size(); i++) {
            if (function_symbol->args[i]->type->type != args[i]->type) {
                if (!(function_symbol->args[i]->type->type == "int" && args[i]->type == "byte")) {
                    break;
                }
            }
        }
        if (i == args.size()) {
            match_count++;
            res_func = function_symbol;
        }
    }
    if (match_count > 1) {
        output::errorAmbiguousCall(yylineno, name);
        exit(0);
    }
    else if (match_count == 0) {
        if (found) {
            output::errorPrototypeMismatch(yylineno, name);
            exit(0);
        }
        else {
            output::errorUndefFunc(yylineno, name);
            exit(0);
        }
    }
    return res_func;
}

bool SymbolTableStack::is_loop() {
    for (auto symbol_table : symbol_tables) {
        if (symbol_table->is_loop) {
            return true;
        }
    }
    return false;
}

bool SymbolTableStack::verify_return_type(string type) {
    for (auto it = symbol_tables.rbegin(); it != symbol_tables.rend(); ++it) {
        if ((*it)->return_type == type) {
            return true;
        }
        if ((*it)->return_type != "") {
            return false;
        }
    }
    return false;
}

void verify_bool(Node* expr) {
    auto exp = dynamic_cast<Exp*>(expr);
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}
