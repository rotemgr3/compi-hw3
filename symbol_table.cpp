#include "symbol_table.h"
#include "hw3_output.hpp"
#include <algorithm>


extern int yylineno;

void SymbolTable::push_symbol(string type, string name, int offset){
    verify_new_symbol(name);
    shared_ptr<Symbol> new_symbol = make_shared<Symbol>(type, name, offset);
    symbols.push_back(new_symbol);
}

void SymbolTable::push_function_symbol(string type, string name, vector<shared_ptr<Formaldecl>> args){
    verify_new_symbol(name);
    shared_ptr<Symbol> new_symbol = make_shared<Symbol>(type, name, 0, args);
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

void SymbolTableStack::push_symbol(string type, string name, vector<shared_ptr<Formaldecl>> args){
    if (type != "function") {
        int offset = offsets.back();
        symbol_tables.back()->push_symbol(type, name, offset);
        offsets.back()++;
        return;
    }
    symbol_tables.back()->push_function_symbol(type, name, args);
    reverse(args.begin(), args.end());
    push_symbol_table();
    int offset = -1 * args.size();
    for (auto arg : args){
        symbol_tables.back()->push_symbol(arg->type->type, arg->id, offset);
        offset++;
    }
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
