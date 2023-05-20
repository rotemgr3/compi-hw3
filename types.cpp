#include "types.h"
#include "hw3_output.hpp"


SymbolTableStack symbol_table_stack = SymbolTableStack();

Funcs::Funcs(Funcdecl* funcdec, Funcs* funcs) {
    funcs_list.push_back(make_shared<Funcdecl>(funcdec));
    if (funcs != nullptr) {
        funcs_list.insert(funcs_list.end(), funcs->funcs_list.begin(), funcs->funcs_list.end());
    }
}

Funcdecl::Funcdecl(Override* override, Rettype* return_type, Node* id, Formals* params) {
    is_override = override->is_override;
    ret_type = make_shared<Rettype>(return_type);
    this->id = id->text;
    formals = params->formals_list->list;
    symbol_table_stack.push_function_symbol(make_shared<Funcdecl>(this));
}