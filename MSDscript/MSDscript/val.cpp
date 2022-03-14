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

NumVal::NumVal(int rep){
    this -> rep = rep;
}

bool NumVal::equals(Val *v){
    NumVal *target = dynamic_cast<NumVal*>(v);
    if (target == NULL) return false;
    return ((this -> rep) == (target -> rep));
}

Expr* NumVal::to_expr(){
    return new NumExpr(this -> rep);
}

std::string NumVal::to_string(){
    return std::to_string(this -> rep);
}

Val* NumVal::add_to(Val* v){
    NumVal *target = dynamic_cast<NumVal*>(v);
    if (target == nullptr) {
        throw std::runtime_error("Error: rhs of Val add_to() must be a NumVal.");
    }
    return new NumVal((this -> rep) + (target -> rep));
}

Val* NumVal::mult_by(Val* v){
    NumVal *target = dynamic_cast<NumVal*>(v);
    if (target == nullptr) {
        throw std::runtime_error("Error: rhs of Val mult_by() must be a NumVal.");
    }
    return new NumVal((this -> rep) * (target -> rep));
}

Val* NumVal::call(Val* arg){
    throw std::runtime_error("Error: call() is unavailable for a NumVal.");
}


// subclass 2: BoolVal

BoolVal::BoolVal(bool val){
    this -> rep = val;
}

bool BoolVal::equals(Val *v){
    BoolVal *target = dynamic_cast<BoolVal*>(v);
    if (target == NULL) return false;
    return ((this -> rep) == (target -> rep));
}


Expr* BoolVal::to_expr(){
    return NULL;
}

std::string BoolVal::to_string(){
    if (this -> rep)
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

Val* BoolVal::call(Val* arg){
    throw std::runtime_error("Error: call() is unavailable for a BoolVal.");
}



// subclass 3: FunVal

FunVal::FunVal(std::string formal_arg, Expr* body){
    this -> formal_arg = formal_arg;
    this -> body = body;
}

bool FunVal::equals(Val *val) {
    FunVal *target = dynamic_cast<FunVal*>(val);
    if (target == NULL) return false;
    else {
        return (this -> formal_arg == target -> formal_arg &&
                this -> body -> equals(target -> body));
    }
}

Expr* FunVal::to_expr() {
    return new FunExpr(this -> formal_arg, this -> body);
}

std::string FunVal::to_string() {
    return ("_fun (" + this -> formal_arg + ") " +
            this -> body -> to_string());
}

Val* FunVal::add_to(Val* v) {
    throw std::runtime_error("Error: FunVal cannot be added.");
}

Val* FunVal::mult_by(Val* v) {
    throw std::runtime_error("Error: FunVal cannot be multiplied.");
}

Val* FunVal::call(Val *arg) {
    LetExpr* let = new LetExpr(new VarExpr(this -> formal_arg),
                               arg -> to_expr(),
                               this -> body);
    return let->interp();
}
