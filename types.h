#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "symbol_table.h"

using namespace std;

class Node {
public:
    string text;

    Node() = default;
    Node(string text) : text(text) {};
    Node(const Node &node): text(node.text) {};
    virtual ~Node() = default;
};

#define YYSTYPE Node*

class Program : public Node {
    Program() = default;
    virtual ~Program() = default;
};

class Funcs : public Node {
    public:
        vector<shared_ptr<Funcdecl>> funcs_list;

        Funcs() = default;
        Funcs(Funcdecl* funcdec, Funcs* funcs);
        virtual ~Funcs() = default;
};

class Rettype : public Node {
    public:
        shared_ptr<Type> type; // If nullptr then void
        
        Rettype() = default;
        Rettype(Type* type) : type(type) {};
        virtual ~Rettype() = default;
};

class Funcdecl : public Node {
    public:
        bool is_override;
        shared_ptr<Rettype> ret_type;
        string id;
        vector<shared_ptr<Formaldecl>> formals;

        Funcdecl(Override* override, Rettype* return_type, Node* id, Formals* params);
        virtual ~Funcdecl() = default;
};

class Override : public Node {
    public:
        bool is_override;
        Override(bool is_override);
        virtual ~Override() = default;
};

class Formals : public Node {
    public:
        shared_ptr<Formalslist> formals_list;

        Formals() = default;
        Formals(Formalslist* formals_list);
        virtual ~Formals() = default;
};

class Formalslist : public Node {
public:
    vector<shared_ptr<Formaldecl>> list;

    Formalslist(Formaldecl* formaldecl);
    Formalslist(Formaldecl* formaldec, Formalslist* formals_list);
    virtual ~Formalslist() = default;
};


class Formaldecl : public Node {
    public:
        Type* type;
        string id;

        Formaldecl(Type* type, Node* id);
        virtual ~Formaldecl() = default;
};

class Statements : public Node {
    public:
        Statements() = default;
        virtual ~Statements() = default;
};

class Statement : public Node {
    public:
        Statement(Statements* statements);
        Statement(Type* type, Node* id);
        Statement(Type* type, Node* id, Exp* exp);
        Statement(Node* str, Exp* exp); // str = id or return
        Statement(Call* call);
        Statement(Node* str); // str = break or continue or return
        Statement(Exp* exp, Statement* statement, Node* str); // str = if or while
        Statement(Exp* exp, Statement* statement1, Statement* statement2); // if else rule
        virtual ~Statement() = default;
};

class Call : public Node {
    public:
        string id;
        Explist* exp_list;

        Call(Node* terminal);
        Call(Node* terminal, Explist* exp_list);
        virtual ~Call() = default;
};

class Explist : public Node {
    public:
        vector<shared_ptr<Exp>> expressions;

        Explist(Node* exp, Node* exp_list);
        Explist(Node* exp);
        virtual ~Explist() = default;
};


class Type : public Node {
    public:
        string type;

        Type(string type) : Node(type), type(type) {};
        Type(Type* type) : type(type->type) {};
        virtual ~Type() = default;
};


class Exp : public Node {
    public:
        // Not sure about members variables 
        string type;
        string value;
        bool is_var=false;

        Exp(Exp *exp);
        Exp(Exp *exp1, Node* op, Exp *exp2);
        Exp(Node* str); // str = id or num or string true or false
        Exp(Call* call);
        Exp(Node* str1, Node* byte);
        Exp(Node* str1, Exp* exp); // str = not
        Exp(Type* type, Node* exp);
        virtual ~Exp() = default;
};


void verify_bool(Node* node);

#endif
