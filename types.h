#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

class Funcs;
class Funcdecl;
class Override;
class Rettype;
class Formaldecl;
class Formals;
class Type;
class formalslist;
class Exp;
class Explist;
class program;
class Statements;
class Statement;
class Call;

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
    public:
        Program();
        virtual ~Program() = default;
};

class Funcs : public Node {
    public:
        Funcs() = default;
        virtual ~Funcs() = default;
};

class Rettype : public Node {
    public:
        shared_ptr<Type> type; 
        
        Rettype() : type(make_shared<Type>("void")) {};
        Rettype(Type* type) : type(type) {};
        Rettype(const Rettype& ret_type) : type(ret_type.type) {};
        virtual ~Rettype() = default;
};

class Funcdecl : public Node {
    public:
        bool is_override;
        shared_ptr<Rettype> ret_type;
        string id;
        vector<shared_ptr<Formaldecl>> formals;

        Funcdecl(Override* override, Rettype* return_type, Node* id, Formals* params);
        Funcdecl(shared_ptr<Override> override, shared_ptr<Rettype> return_type, shared_ptr<Node> id, shared_ptr<Formals> params);
        Funcdecl(const Funcdecl& funcdecl) : is_override(funcdecl.is_override), ret_type(funcdecl.ret_type), id(funcdecl.id), formals(funcdecl.formals) {};
        virtual ~Funcdecl() = default;
};

class Override : public Node {
    public:
        bool is_override;

        Override(bool is_override) : is_override(is_override) {};
        virtual ~Override() = default;
};

class Formaldecl : public Node {
    public:
        shared_ptr<Type> type;
        string id;

        Formaldecl(Type* type, Node* id);
        Formaldecl(shared_ptr<Type> type, shared_ptr<Node> id) : type(type), id(id->text) {};
        virtual ~Formaldecl() = default;
};

class Formalslist : public Node {
public:
    vector<shared_ptr<Formaldecl>> list;

    Formalslist() = default;
    Formalslist(Formaldecl* formaldecl);
    Formalslist(shared_ptr<Formaldecl> formaldecl);
    Formalslist(Formaldecl* formaldec, Formalslist* formals_list);
    virtual ~Formalslist() = default;
};

class Formals : public Node {
    public:
        shared_ptr<Formalslist> formals_list;

        Formals() : formals_list(make_shared<Formalslist>()) {};
        Formals(Formalslist* formals_list) : formals_list(formals_list) {};
        Formals(shared_ptr<Formalslist> formals_list) : formals_list(formals_list) {};
        virtual ~Formals() = default;
};

class Statements : public Node {
    public:
        Statements() = default;
        virtual ~Statements() = default;
};


class Type : public Node {
    public:
        string type;

        Type(Node* type) : type(type->text) {};
        Type(string type) : type(type) {};
        virtual ~Type() = default;
};


class Exp : public Node {
    public:
        string type;
        string value;
        bool is_var = false;

        Exp(Exp* exp);
        Exp(Exp* exp1, Node* op, Exp* exp2);
        Exp(Node* str); // str = id or num or string true or false
        Exp(Call* call);
        Exp(Node* str1, Node* byte);
        Exp(Node* str1, Exp* exp); // str = not
        Exp(Type* type, Exp* exp);
        virtual ~Exp() = default;
};

class Statement : public Node {
    public:
        Statement(Type* type, Node* id);
        Statement(Type* type, Node* id, Exp* exp);
        Statement(Node* str, Exp* exp); // str = id or return
        Statement(Node* str); // str = break or continue or return
        virtual ~Statement() = default;
};

class Call : public Node {
    public:
        string id;
        shared_ptr<Explist> exp_list;
        string ret_type;

        Call(Node* id);
        Call(Node* id, Explist* exp_list);
        virtual ~Call() = default;
};

class Explist : public Node {
    public:
        vector<shared_ptr<Exp>> expressions;

        Explist(Exp* exp, Explist* exp_list);
        Explist(Exp* exp);
        Explist() = default;
        virtual ~Explist() = default;
};


void verify_bool(Node* node);

#endif
