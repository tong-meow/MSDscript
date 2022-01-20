//
//  expr.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/17/22.
//

#include <iostream>
#include <stdexcept>
#include "expr.hpp"


// subclass 1: Num

Num::Num(int val){
    this -> val = val;
}

bool Num::equals(Expr *e){
    Num *target = dynamic_cast<Num*>(e);
    if (target == NULL) return false;
    return ((this -> val) == (target -> val));
}

int Num::interp(){
    return this -> val;
}

bool Num::has_variable(){
    return false;
}

Expr* Num::subst(std::string, Expr *e){
    return this;
}



// subclass 2: Add

Add::Add(Expr* lhs, Expr* rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool Add::equals(Expr *e){
    Add *target = dynamic_cast<Add*>(e);
    if (target == NULL) return false;
    return (((this->lhs) -> equals (target->lhs))
            && ((this->rhs) -> equals (target->rhs)));
}

int Add::interp(){
    // since lhs and rhs could be Num, Add, Mult or Variable, calculate their
    // values recursively.
    // if one of the element inside is a Variable, exception will be throwed in
    // the Variable::interp().
    return (this -> lhs) -> interp() + (this -> rhs) -> interp();
}

bool Add::has_variable(){
    return (lhs -> has_variable() || rhs -> has_variable());
}

Expr* Add::subst(std::string s, Expr *e){
    return (new Add((this-> lhs) -> subst(s, e), (this-> rhs) -> subst(s, e)));
}



// subclass 3: Mult

Mult::Mult(Expr* lhs, Expr* rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool Mult::equals(Expr *e){
    Mult *target = dynamic_cast<Mult*>(e);
    if (target == NULL) return false;
    return (((this->lhs) -> equals (target->lhs))
            && ((this->rhs) -> equals (target->rhs)));
}

int Mult::interp(){
    // since lhs and rhs could be Num, Add, Mult or Variable, calculate their
    // values recursively.
    // if one of the element inside is a Variable, exception will be throwed in
    // the Variable::interp().
    return (this -> lhs) -> interp() * (this -> rhs) -> interp();
}

bool Mult::has_variable(){
    return (lhs -> has_variable() || rhs -> has_variable());
}

Expr* Mult::subst(std::string s, Expr *e){
    return (new Mult((this-> lhs) -> subst(s, e), (this-> rhs) -> subst(s, e)));
}



// subclass 4: Variable

Variable::Variable(std::string str){
    this -> str = str;
}

bool Variable::equals(Expr *e){
    Variable *target = dynamic_cast<Variable*>(e);
    if (target == NULL) return false;
    return ((this->str) == (target->str));
}

int Variable::interp(){
    // throw exception since there is no integer value for a string
    throw std::runtime_error("Error: Expr contains a string element.");
}

bool Variable::has_variable(){
    return true;
}

Expr* Variable::subst(std::string s, Expr *e){
    if ((this -> str) == s){
        return e;
    }else{
        return this;
    }
}

