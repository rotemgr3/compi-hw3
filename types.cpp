#include "types.h"
#include "hw3_output.hpp"


Funcs::Funcs(Funcdecl* funcdec, Funcs* funcs) {
    funcs_list.push_back(make_shared<Funcdecl>(funcdec));
    if (funcs != nullptr) {
        funcs_list.insert(funcs_list.end(), funcs->funcs_list.begin(), funcs->funcs_list.end());
    }
}

Funcdecl::Funcdecl(Override* override, Rettype* return_type, Node* id, Formals* params) {
    is_override = override->is_override;
    ret_type = return_type->type;
    this->id = id->text;
    formals = params->formals_list->list;

}