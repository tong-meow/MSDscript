//
//  val.cpp
//  MSDscript
//
//  Created by Tong Shen on 2/16/22.
//

#include <string>
#include <iostream>

#include "val.hpp"
#include "expr.hpp"


// subclass 1: NumVal

NumVal::NumVal(int val){
    this -> val = val;
}

bool NumVal::equals(Val *v){
    NumVal *target = dynamic_cast<NumVal*>(v);
    if (target == NULL) return false;
    return ((this -> val) == (target -> val));
}

Expr* NumVal::to_expr(){
    return new NumExpr(this -> val);
}

std::string NumVal::to_string(){
    return std::to_string(this -> val);
}

Val* NumVal::add_to(Val* v){
    NumVal *target = dynamic_cast<NumVal*>(v);
    if (target == nullptr) {
        throw std::runtime_error("Error: rhs of Val add_to() must be a NumVal.");
    }
    return new NumVal((this -> val) + (target -> val));
}

Val* NumVal::mult_by(Val* v){
    NumVal *target = dynamic_cast<NumVal*>(v);
    if (target == nullptr) {
        throw std::runtime_error("Error: rhs of Val mult_by() must be a NumVal.");
    }
    return new NumVal((this -> val) * (target -> val));
}


// subclass 2: BoolVal

BoolVal::BoolVal(bool val){
    this -> val = val;
}

bool BoolVal::equals(Val *v){
    BoolVal *target = dynamic_cast<BoolVal*>(v);
    if (target == NULL) return false;
    return ((this -> val) == (target -> val));
}


Expr* BoolVal::to_expr(){
    return NULL;
}

std::string BoolVal::to_string(){
    if (this -> val)
        return "_true";
    else
        return "_false";
}

Val* BoolVal::add_to(Val* v){
    throw std::runtime_error("Error: BoolVal cannot be added.");
}

Val* BoolVal::mult_by(Val* v){
    throw std::runtime_error("Error: BoolVal cannot be multiplied.");
}
