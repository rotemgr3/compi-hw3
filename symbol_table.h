#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "types.h"

using namespace std;

class Symbol{
    public:
        string type;
        string name;
        int offset;

        Symbol(string type, string name, int offset) : type(type), name(name), offset(offset) {};
        Symbol(const Symbol &symbol) : type(symbol.type), name(symbol.name), offset(symbol.offset) {};
        ~Symbol() = default;
};

class FunctionSymbol : public Symbol{
    public:
        vector<shared_ptr<Formaldecl>> args;

        FunctionSymbol(string type, string name, int offset, vector<shared_ptr<Formaldecl>> args) : Symbol(type, name, offset), args(args) {};
        FunctionSymbol(const FunctionSymbol &function_symbol) : Symbol(function_symbol), args(function_symbol.args) {};
        ~FunctionSymbol() = default;
};

class SymbolTable{
    public:
        vector<shared_ptr<Symbol>> symbols;

        SymbolTable() = default;
        SymbolTable(const SymbolTable &symbol_table) : symbols(symbol_table.symbols) {};
        ~SymbolTable() = default;
        void push_symbol(string type, string name, int offset);
        void verify_new_symbol(string name);
        shared_ptr<Symbol> get_symbol(string name);
        void push_function_symbol(string type, string name, vector<shared_ptr<Formaldecl>> args);
};

class SymbolTableStack{
    public:
        vector<shared_ptr<SymbolTable>> symbol_tables;
        vector<int> offsets;

        SymbolTableStack() = default;
        SymbolTableStack(const SymbolTableStack &symbol_table_stack) : symbol_tables(symbol_table_stack.symbol_tables) {};
        ~SymbolTableStack() = default;
        void push_symbol_table();
        void pop_symbol_table();
        void push_symbol(string type, string name, vector<shared_ptr<Formaldecl>> args = vector<shared_ptr<Formaldecl>>());
        shared_ptr<Symbol> get_symbol(string name);
};


#endif // SYMBOL_TABLE_H