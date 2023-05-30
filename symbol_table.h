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
        virtual ~Symbol() = default;
};

class FunctionSymbol : public Symbol{
    public:
        vector<shared_ptr<Formaldecl>> args;
        bool is_override;
        shared_ptr<Rettype> ret_type;

        FunctionSymbol(string type, string name, vector<shared_ptr<Formaldecl>> args, bool is_override, shared_ptr<Rettype> ret_type) : 
        Symbol(type, name, /*offset=*/0), args(args), is_override(is_override),  ret_type(ret_type) {};
        FunctionSymbol(const FunctionSymbol &function_symbol) : 
            Symbol(function_symbol), args(function_symbol.args), is_override(function_symbol.is_override), ret_type(function_symbol.ret_type) {};
        ~FunctionSymbol() = default;
};

class SymbolTable{
    public:
        vector<shared_ptr<Symbol>> symbols;
        bool is_loop = false;
        string return_type;

        SymbolTable() = default;
        SymbolTable(bool is_loop, string return_type) : symbols(), is_loop(is_loop), return_type(return_type) {};
        SymbolTable(const SymbolTable &symbol_table) : symbols(symbol_table.symbols), is_loop(symbol_table.is_loop), return_type(symbol_table.return_type) {};
        ~SymbolTable() = default;
        void push_symbol(string type, string name, int offset);
        void push_function_symbol(shared_ptr<FunctionSymbol> new_symbol);
        void verify_new_symbol(string name);
        vector<shared_ptr<Symbol>>::iterator verify_new_function_symbol(shared_ptr<FunctionSymbol> new_symbol);
        shared_ptr<Symbol> get_symbol(string name);
};

class SymbolTableStack{
    public:
        vector<shared_ptr<SymbolTable>> symbol_tables;
        vector<int> offsets;
        bool found_main = false;

        SymbolTableStack();
        SymbolTableStack(const SymbolTableStack &symbol_table_stack) : symbol_tables(symbol_table_stack.symbol_tables) {};
        ~SymbolTableStack() = default;
        void push_symbol_table(bool is_loop = false, string return_type = "");
        void pop_symbol_table();
        void push_symbol(string type, string name);
        void push_function_symbol(shared_ptr<Funcdecl> funcdecl, bool is_decleration_only = false);
        void verify_new_symbol(string name, vector<shared_ptr<Formaldecl>> args = vector<shared_ptr<Formaldecl>>());
        vector<shared_ptr<Symbol>>::iterator verify_new_function_symbol(shared_ptr<FunctionSymbol> new_symbol);
        shared_ptr<Symbol> get_symbol(string name);
        shared_ptr<FunctionSymbol> match_function_symbol(string name, vector<shared_ptr<Exp>> args);
        void verify_symbol(string name);
        void verify_main();
        shared_ptr<SymbolTable> get_current_symbol_table();
        bool is_loop();
        bool verify_return_type(string type);

};

void verify_bool(Node* expr);

#endif // SYMBOL_TABLE_H