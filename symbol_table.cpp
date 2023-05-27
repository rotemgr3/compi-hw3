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
            // TODO: check if it can happen
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

void SymbolTableStack::push_symbol_table(){
    shared_ptr<SymbolTable> new_symbol_table = make_shared<SymbolTable>();
    symbol_tables.push_back(new_symbol_table);
    if (offsets.empty()){
        offsets.push_back(0);
    }
    else{
        offsets.push_back(offsets.back());
    }
}

void SymbolTableStack::pop_symbol_table(){
    shared_ptr<SymbolTable> symbols = symbol_tables.back();
    symbol_tables.pop_back();
    offsets.pop_back();
    output::endScope();
    for (auto symbol : symbols->symbols){
        output::printID(symbol->name, symbol->offset, symbol->type);
    }
}

void SymbolTableStack::push_symbol(string type, string name){
    int offset = offsets.back();
    symbol_tables.back()->push_symbol(type, name, offset);
    offsets.back()++;
    return;
    
}

void SymbolTableStack::push_function_symbol(shared_ptr<Funcdecl> funcdecl){
    if (funcdecl->id == "main" && funcdecl->ret_type->type->type == "void" && funcdecl->formals.size() == 0){
        found_main = true;
    }
    shared_ptr<FunctionSymbol> new_symbol = make_shared<FunctionSymbol>(funcdecl->ret_type->type->type, funcdecl->id, funcdecl->formals, funcdecl->is_override, funcdecl->ret_type);
    auto old_symbol_it = verify_new_function_symbol(new_symbol);
    symbol_tables.back()->push_function_symbol(new_symbol);
    push_symbol_table();
    int offset = -1 * funcdecl->formals.size();
    for(auto it=funcdecl->formals.end(); it!=funcdecl->formals.begin(); it--){
        symbol_tables.back()->push_symbol((*it)->type->type, (*it)->id, offset);
        offset++;
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
    output::errorUndef(yylineno, name);
    exit(0);
}


void verify_bool(Node* expr) {
    return;
}
