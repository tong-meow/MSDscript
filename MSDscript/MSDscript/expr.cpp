//
//  expr.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/17/22.
//

#include <iostream>
#include <sstream>
#include <stdexcept>
#include "expr.hpp"


/*//////////////////NON-VIRTUAL FUNCTIONS//////////////////*/

// to_string function uses print() to return a string version of an Expr
std::string Expr::to_string(){
    std::stringstream ss;
    this -> print(ss);
    return ss.str();
}

// the pretty_print function uses helper preety_print_at() function to print an Expr
void Expr::pretty_print(std::ostream &os){
    this -> pretty_print_at(os, false, false, false);
}

// to_pretty_string function uses pretty_print() to return a string version of an Expr
std::string Expr::to_pretty_string(){
    std::stringstream ss;
    this -> pretty_print(ss);
    return ss.str();
}




/*//////////////////SUBCLASSES//////////////////*/

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

void Num::print(std::ostream& os){
    os << (this -> val);
}

void Num::pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs){
    os << (this -> val);
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
    return (this -> lhs) -> interp() + (this -> rhs) -> interp();
}

bool Add::has_variable(){
    return (lhs -> has_variable() || rhs -> has_variable());
}

Expr* Add::subst(std::string s, Expr *e){
    return (new Add((this-> lhs) -> subst(s, e), (this-> rhs) -> subst(s, e)));
}

void Add::print(std::ostream& os){
    os << '(';
    (this->lhs) -> print(os);
    os << '+';
    (this->rhs) -> print(os);
    os << ')';
}

void Add::pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs){
    // there are 2 occasions that Add doesn't need ()
    // 1. it is at the top level
    // 2. it is inside a '+' but at the right side. e.g. 1 + 2 + 3, the '2 + 3' needs no ()
    if ((!insideAdd && !insideMult) || (insideAdd && !lhs)){
        (this -> lhs) -> pretty_print_at(os, true, false, true);
        os << " + ";
        (this -> rhs) -> pretty_print_at(os, true, false, false);
    }else{
        os << '(';
        (this -> lhs) -> pretty_print_at(os, true, false, true);
        os << " + ";
        (this -> rhs) -> pretty_print_at(os, true, false, false);
        os << ')';
    }
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
    return (this -> lhs) -> interp() * (this -> rhs) -> interp();
}

bool Mult::has_variable(){
    return (lhs -> has_variable() || rhs -> has_variable());
}

Expr* Mult::subst(std::string s, Expr *e){
    return (new Mult((this-> lhs) -> subst(s, e), (this-> rhs) -> subst(s, e)));
}

void Mult::print(std::ostream& os){
    os << '(';
    (this->lhs) -> print(os);
    os << '*';
    (this->rhs) -> print(os);
    os << ')';
}

void Mult::pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs){
    // there is only 1 occasion that Mult needs ()
    // 1. it is inside a mult and is at the left side
    //    e.g. (3 * 2) * 1, the (3 * 2) needs ()
    if (insideMult && lhs){
        os << '(';
        (this -> lhs) -> pretty_print_at(os, false, true, true);
        os << " * ";
        (this -> rhs) -> pretty_print_at(os, false, true, false);
        os << ')';
    }else{
        (this -> lhs) -> pretty_print_at(os, false, true, true);
        os << " * ";
        (this -> rhs) -> pretty_print_at(os, false, true, false);
    }
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

void Variable::print(std::ostream& os){
    os << (this->str);
}

void Variable::pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs){
    os << (this->str);
}
