//
//  val.cpp
//  MSDscript
//
//  Created by Tong Shen on 2/16/22.
//


#include <string>
#include <iostream>

#include "val.hpp"
#include "env.hpp"
#include "expr.hpp"
#include "parse.hpp"
#include "step.hpp"


// subclass 1: NumVal

NumVal::NumVal(int rep){
    this -> rep = rep;
}

bool NumVal::equals(PTR(Val) v){
    PTR(NumVal) target = dynamic_cast<PTR(NumVal)>(v);
    if (target == NULL) return false;
    return ((this -> rep) == (target -> rep));
}

PTR(Expr) NumVal::to_expr(){
    return new NumExpr(this -> rep);
}

std::string NumVal::to_string(){
    return std::to_string(this -> rep);
}

PTR(Val) NumVal::add_to(PTR(Val) v){
    PTR(NumVal) target = dynamic_cast<PTR(NumVal)>(v);
    if (target == nullptr) {
        throw std::runtime_error("Error: rhs of Val add_to() must be a NumVal.");
    }
    return new NumVal((this -> rep) + (target -> rep));
}

PTR(Val) NumVal::mult_by(PTR(Val) v){
    PTR(NumVal) target = dynamic_cast<PTR(NumVal)>(v);
    if (target == nullptr) {
        throw std::runtime_error("Error: rhs of Val mult_by() must be a NumVal.");
    }
    return new NumVal((this -> rep) * (target -> rep));
}

PTR(Val) NumVal::call(PTR(Val) arg){
    throw std::runtime_error("Error: call() is unavailable for a NumVal.");
}

bool NumVal::is_true() {
    return false;
}

void NumVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {}


// subclass 2: BoolVal

BoolVal::BoolVal(bool val){
    this -> rep = val;
}

bool BoolVal::equals(PTR(Val) v){
    PTR(BoolVal) target = dynamic_cast<PTR(BoolVal)>(v);
    if (target == NULL) return false;
    return ((this -> rep) == (target -> rep));
}


PTR(Expr) BoolVal::to_expr(){
    return NULL;
}

std::string BoolVal::to_string(){
    if (this -> rep)
        return "_true";
    else
        return "_false";
}

PTR(Val) BoolVal::add_to(PTR(Val) v){
    throw std::runtime_error("Error: BoolVal cannot be added.");
}

PTR(Val) BoolVal::mult_by(PTR(Val) v){
    throw std::runtime_error("Error: BoolVal cannot be multiplied.");
}

PTR(Val) BoolVal::call(PTR(Val) arg){
    throw std::runtime_error("Error: call() is unavailable for a BoolVal.");
}

bool BoolVal::is_true() {
    return rep;
}

void BoolVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {}


// subclass 3: FunVal
FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env){
    this -> formal_arg = formal_arg;
    this -> body = body;
    this -> env = env;
}

bool FunVal::equals(PTR(Val) val) {
    PTR(FunVal) target = dynamic_cast<PTR(FunVal)>(val);
    if (target == NULL) return false;
    else {
        return (this -> formal_arg == target -> formal_arg &&
                this -> body -> equals(target -> body));
    }
}

PTR(Expr) FunVal::to_expr() {
    return new FunExpr(this -> formal_arg, this -> body);
}

std::string FunVal::to_string() {
    return ("_fun (" + this -> formal_arg + ") " +
            this -> body -> to_string());
}

PTR(Val) FunVal::add_to(PTR(Val) v) {
    throw std::runtime_error("Error: FunVal cannot be added.");
}

PTR(Val) FunVal::mult_by(PTR(Val) v) {
    throw std::runtime_error("Error: FunVal cannot be multiplied.");
}

PTR(Val) FunVal::call(PTR(Val) arg) {
    return body -> interp(new ExtendedEnv(this -> formal_arg,
                                          arg,
                                          this -> env));
}

bool FunVal::is_true() {
    return false;
}

void FunVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(formal_arg, actual_arg_val, env);
    Step::cont = rest;
}
