#include "types.h"
#include "symbol_table.h"
#include "hw3_output.hpp"
#include <string.h>


extern SymbolTableStack symbol_table_stack;
extern int yylineno;


Funcs::Funcs(Funcdecl* funcdec, Funcs* funcs) : funcs_list() {
    funcs_list.push_back(make_shared<Funcdecl>(*funcdec));
    if (funcs != nullptr) {
        funcs_list.insert(funcs_list.end(), funcs->funcs_list.begin(), funcs->funcs_list.end());
    }
    symbol_table_stack.verify_main();
}

Funcdecl::Funcdecl(Override* override, Rettype* return_type, Node* id, Formals* params) {
    is_override = override->is_override;
    ret_type = make_shared<Rettype>(*return_type);
    this->id = id->text;
    formals = params->formals_list->list;
    symbol_table_stack.push_function_symbol(make_shared<Funcdecl>(*this));
}

Formalslist::Formalslist(Formaldecl* formaldecl) : list() {
    list.push_back(make_shared<Formaldecl>(*formaldecl));
}

Formalslist::Formalslist(Formaldecl* formaldecl, Formalslist* formalslist) : list() {
    list.push_back(make_shared<Formaldecl>(*formaldecl));
    if (formalslist != nullptr)
        list.insert(list.end(), formalslist->list.begin(), formalslist->list.end());
}

Exp::Exp(Exp* exp) : type(exp->type), value(exp->value), is_var(exp->is_var) {}

Exp::Exp(Exp* exp1, Node* op, Exp* exp2){
    if (exp1->is_var){
        symbol_table_stack.verify_symbol(exp1->value);
    }
    if (exp2->is_var){
        symbol_table_stack.verify_symbol(exp2->value);
    }
    if (op->text == "+" || op->text == "-" || op->text == "*" || op->text == "/") {
        if (exp1->type != "int" || exp1->type != "byte") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if (exp2->type != "int" || exp2->type != "byte") {
            output::errorMismatch(yylineno);
            exit(0);
        }

        if (exp1->type == "int" || exp2->type == "int") {
            type = "int";
        }
        else {
            type = "byte";
        }
    }
    else if (op->text == "and" || op->text == "or") {
        if (exp1->type != "bool") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if (exp2->type != "bool") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        type = "bool";
    }
    else { // ==|!=|<|>|<=|>=   
        if (exp1->type != "int" || exp1->type != "byte") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if (exp2->type != "int" || exp2->type != "byte") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        type = "bool";
    }
}

// str = id or num or string or true or false
Exp::Exp(Node* str) : value(str->text) {
    if (str->text == "true" || str->text == "false"){
        type = "bool";
        value = str->text;
    }
    if (str->text[0] == '"') {
        type = "string";
    }
    if (isdigit(str->text[0])) {
        type = "int";
    }
    else {
        symbol_table_stack.verify_symbol(str->text);
        type = symbol_table_stack.get_symbol(str->text)->type;
        is_var = true;
    }   
}
Exp::Exp(Call* call) {
    symbol_table_stack.verify_symbol(call->id);
    type = dynamic_cast<FunctionSymbol*>(symbol_table_stack.get_symbol(call->id).get())->ret_type->type->type;
}

Exp::Exp(Node* str1, Node* byte){
    type = "byte";
    int value = stoi(str1->text);
    if (value > 255) {
        output::errorByteTooLarge(yylineno, str1->text);
        exit(0);
    }
}
// str = not
Exp::Exp(Node* str1, Exp* exp){
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    type = "bool";
}

Exp::Exp(Type* type, Exp* exp){
    if (exp->is_var){
        symbol_table_stack.verify_symbol(exp->value);
    }
    if (type->type == "int" || type->type == "byte") {
        if (exp->type != "int" && exp->type != "byte") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        this->type = type->type;
        this->value = exp->value;
    }
    else if (type->type == exp->type){
        this->type = type->type;
        this->value = exp->value;
    }
    else {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

Statement::Statement(Type* type, Node* id) {
    symbol_table_stack.push_symbol(type->type, id->text);
}

Statement::Statement(Type* type, Node* id, Exp* exp){
    if (exp->is_var){
        symbol_table_stack.verify_symbol(exp->value);
    }
    if (type->type != exp->type) {
        if (!(type->type == "int" && exp->type == "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }  
    }
    symbol_table_stack.push_symbol(type->type, id->text);
}

// str = id or return
Statement::Statement(Node* str, Exp* exp) {
    if (str->text == "return") {
        if (exp->is_var){
            symbol_table_stack.verify_symbol(exp->value);
        }
        auto symbol_table = symbol_table_stack.get_current_symbol_table();
        if (symbol_table->return_type == "void") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if(symbol_table->return_type != exp->type){
             if (!(symbol_table->return_type == "int" && exp->type == "byte")) {
                output::errorMismatch(yylineno);
                exit(0);
            }  
        }
    }
    else {
        symbol_table_stack.verify_symbol(str->text);
        auto symbol = symbol_table_stack.get_symbol(str->text);
        if (symbol->type != exp->type || symbol->type == "function") {
            if (!(symbol->type == "int" && exp->type == "byte")) {
                output::errorMismatch(yylineno);
                exit(0);
            }  
        }
    }
}

Statement::Statement(Call* call){
    
}
Statement::Statement(Node* str); // str = break or continue or return
Statement::Statement(Exp* exp, Statement* statement, Node* str); // str = if or while
Statement::Statement(Exp* exp, Statement* statement1, Statement* statement2); // if else rule