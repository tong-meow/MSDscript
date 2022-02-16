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
#include "val.hpp"


/*//////////////////NON-VIRTUAL FUNCTIONS//////////////////*/

// to_string function uses print() to return a string version of an Expr
std::string Expr::to_string(){
    std::stringstream ss;
    this -> print(ss);
    return ss.str();
}

// the pretty_print function uses helper preety_print_at() function to print an Expr
void Expr::pretty_print(std::ostream &os){
//    this -> pretty_print_at(os, false, false, false);
    this -> pretty_print_at(os, prec_none, false, false, 0);
}

// to_pretty_string function uses pretty_print() to return a string version of an Expr
std::string Expr::to_pretty_string(){
    std::stringstream ss;
    this -> pretty_print(ss);
    return ss.str();
}




/*//////////////////SUBCLASSES//////////////////*/

// subclass 1: NumExpr

NumExpr::NumExpr(int val){
    this -> val = val;
}

bool NumExpr::equals(Expr *e){
    NumExpr *target = dynamic_cast<NumExpr*>(e);
    if (target == NULL) return false;
    return ((this -> val) == (target -> val));
}

Val* NumExpr::interp(){
    return new NumVal(this -> val);
}

bool NumExpr::has_variable(){
    return false;
}

Expr* NumExpr::subst(std::string, Expr *e){
    return this;
}

void NumExpr::print(std::ostream& os){
    os << (this -> val);
}

//void NumExpr::pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs){
//    os << (this -> val);
//}
void NumExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                          bool nestedLet, int spaces){
    os << (this -> val);
};



// subclass 2: AddExpr

AddExpr::AddExpr(Expr* lhs, Expr* rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool AddExpr::equals(Expr *e){
    AddExpr *target = dynamic_cast<AddExpr*>(e);
    if (target == NULL) return false;
    return (((this->lhs) -> equals (target->lhs))
            && ((this->rhs) -> equals (target->rhs)));
}

Val* AddExpr::interp(){
    return ((this -> lhs) -> interp())->
            add_to( (this -> rhs) -> interp() );
}

bool AddExpr::has_variable(){
    return (lhs -> has_variable() || rhs -> has_variable());
}

Expr* AddExpr::subst(std::string s, Expr *e){
    return (new AddExpr((this-> lhs) -> subst(s, e), (this-> rhs) -> subst(s, e)));
}

void AddExpr::print(std::ostream& os){
    os << '(';
    (this->lhs) -> print(os);
    os << '+';
    (this->rhs) -> print(os);
    os << ')';
}

//void AddExpr::pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs){
//    // there are 2 occasions that AddExpr doesn't need ()
//    // 1. it is at the top level
//    // 2. it is inside a '+' but at the right side. e.g. 1 + 2 + 3, the '2 + 3' needs no ()
//    if ((!insideAdd && !insideMult) || (insideAdd && !lhs)){
//        (this -> lhs) -> pretty_print_at(os, true, false, true);
//        os << " + ";
//        (this -> rhs) -> pretty_print_at(os, true, false, false);
//    }else{
//        os << '(';
//        (this -> lhs) -> pretty_print_at(os, true, false, true);
//        os << " + ";
//        (this -> rhs) -> pretty_print_at(os, true, false, false);
//        os << ')';
//    }
//}
void AddExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                          bool nestedLet, int spaces){
    long start = os.tellp();
    if (prec < 2 || (prec==2 && !lhs)){
        (this -> lhs) -> pretty_print_at(os, prec_add, true, true, spaces);
        os << " + ";
        long end1 = os.tellp();
        (this -> rhs) -> pretty_print_at(os, prec_add, false, nestedLet,
                                         spaces+(int)(end1-start));
    }else{
        os << '(';
        long end2 = os.tellp();
        (this -> lhs) -> pretty_print_at(os, prec_add, true, true,
                                         spaces+(int)(end2-start));
        os << " + ";
        long end3 = os.tellp();
        (this -> rhs) -> pretty_print_at(os, prec_add, false, nestedLet,
                                         spaces+(int)(end3-start));
        os << ')';
    }
}


// subclass 3: MultExpr

MultExpr::MultExpr(Expr* lhs, Expr* rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool MultExpr::equals(Expr *e){
    MultExpr *target = dynamic_cast<MultExpr*>(e);
    if (target == NULL) return false;
    return (((this->lhs) -> equals (target->lhs))
            && ((this->rhs) -> equals (target->rhs)));
}

Val* MultExpr::interp(){
    return ((this -> lhs) -> interp()) ->
            mult_to ((this -> rhs) -> interp());
}

bool MultExpr::has_variable(){
    return (lhs -> has_variable() || rhs -> has_variable());
}

Expr* MultExpr::subst(std::string s, Expr *e){
    return (new MultExpr((this-> lhs) -> subst(s, e), (this-> rhs) -> subst(s, e)));
}

void MultExpr::print(std::ostream& os){
    os << '(';
    (this->lhs) -> print(os);
    os << '*';
    (this->rhs) -> print(os);
    os << ')';
}

//void MultExpr::pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs){
//    // there is only 1 occasion that MultExpr needs ()
//    // 1. it is inside a MultExpr and is at the left side
//    //    e.g. (3 * 2) * 1, the (3 * 2) needs ()
//    if (insideMult && lhs){
//        os << '(';
//        (this -> lhs) -> pretty_print_at(os, false, true, true);
//        os << " * ";
//        (this -> rhs) -> pretty_print_at(os, false, true, false);
//        os << ')';
//    }else{
//        (this -> lhs) -> pretty_print_at(os, false, true, true);
//        os << " * ";
//        (this -> rhs) -> pretty_print_at(os, false, true, false);
//    }
//}
void MultExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                           bool nestedLet, int spaces){
    long start = os.tellp();
    if (prec >= 3 && lhs){
        os << '(';
        long end1 = os.tellp();
        (this -> lhs) -> pretty_print_at(os, prec_mult, true, true,
                                         spaces+(int)(end1-start));
        os << " * ";
        long end2 = os.tellp();
        (this -> rhs) -> pretty_print_at(os, prec_mult, false, nestedLet,
                                         spaces+(int)(end2-start));
        os << ')';
    }else{
        (this -> lhs) -> pretty_print_at(os, prec_mult, true, true, spaces);
        os << " * ";
        long end3 = os.tellp();
        (this -> rhs) -> pretty_print_at(os, prec_mult, false, nestedLet,
                                         spaces+(int)(end3-start));
    }
}


// subclass 4: VarExpr

VarExpr::VarExpr(std::string str){
    this -> str = str;
}

bool VarExpr::equals(Expr *e){
    VarExpr *target = dynamic_cast<VarExpr*>(e);
    if (target == NULL) return false;
    return ((this->str) == (target->str));
}

Val* VarExpr::interp(){
    // throw exception since there is no integer value for a string
    throw std::runtime_error("Error: Expr contains a string element.");
}

bool VarExpr::has_variable(){
    return true;
}

Expr* VarExpr::subst(std::string s, Expr *e){
    if ((this -> str) == s){
        return e;
    }else{
        return this;
    }
}

void VarExpr::print(std::ostream& os){
    os << (this->str);
}

//void VarExpr::pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs){
//    os << (this->str);
//}
void VarExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                          bool nestedLet, int spaces){
    os << (this->str);
};




// subclass 5: LetExpr
LetExpr::LetExpr(VarExpr* varName, Expr* rhs, Expr* body){
    this -> varName = varName;
    this -> rhs = rhs;
    this -> body = body;
}

bool LetExpr::equals(Expr* e){
    LetExpr *target = dynamic_cast<LetExpr*>(e);
    if (target == NULL) return false;
    return ( (this->varName) -> equals (target->varName)
            && (this->rhs) -> equals (target->rhs)
            && (this->body) -> equals (target->body)
            );
}

Val* LetExpr::interp(){
    // if the rhs has VarExpr, directly accumulate
//    if (rhs->has_variable()){
//        return ((this->body)
//                -> subst (((this->varName)->to_string()), (this->rhs)))
//                -> interp();
//    }
    // if the rhs doesn't have VarExpr, interp the rhs first
//    else{
    Val* rhs_val = this -> rhs -> interp();
    return ((this->body)
            -> subst ((this->varName)->to_string(), rhs_val -> to_expr()))
            -> interp();
//    }
}

bool LetExpr::has_variable(){
    return ((this->rhs)->has_variable() || (this->body)->has_variable());
}

Expr* LetExpr::subst(std::string s, Expr *e){
    // compare string s with varName of the _let
    // if the names are same, use the nearest one
    if (s == (this->varName)->to_string()){
        return new LetExpr(this->varName,
                        this->rhs->subst(s,e),
                        this->body);
    }
    // if not, accumulate it
    else{
        return new LetExpr(this->varName,
                        this->rhs->subst(s, e),
                        this->body->subst(s, e));
    }
}

void LetExpr::print(std::ostream& os){
    os << "(_let ";
    this->varName->print(os);
    os << "=";
    this->rhs->print(os);
    os << " _in ";
    this->body->print(os);
    os << ")";
}

void LetExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                           bool nestedLet, int spaces){
    long start = os.tellp();
    // 2 conditions require parentheses:
    //  - when the LetExpr is the lhs of MultExpr or AddExpr (prec > 1 && lhs == true)
    //  - when the LetExpr is the rhs of MultExpr or AddExpr, but is nested as the lhs of
    //    an outer MultExpr or AddExpr (then the bool nestedLet would be true)
    if ((prec > 1 && lhs) || nestedLet){
        os << "(_let ";
        this->varName->pretty_print_at(os, prec_let, false, false, spaces+1);
        os << " = ";
        long end1 = os.tellp();
        this->rhs->pretty_print_at(os, prec_let, false, false,
                                   spaces+(int)(end1-start));
        os << "\n";
        // spaces + 1: the '1' stands for the '(' in '(_let'
        for(int i=0; i<spaces+1; i++){
            os << " ";
        }
        os << "_in ";
        // spaces + 5: the '5' stands for the '(' + '_in '
        this->body->pretty_print_at(os, prec_let, false, false, spaces+5);
        os << ")";
    }else{
        os << "_let ";
        this->varName->pretty_print_at(os, prec_let, false, false, spaces);
        os << " = ";
        long end2 = os.tellp();
        this->rhs->pretty_print_at(os, prec_let, false, false,
                                   spaces+(int)(end2-start));
        os << "\n";
        for(int i=0; i<spaces; i++){
            os << " ";
        }
        os << "_in ";
        // spaces + 4: the '4' stands for the '_in '
        this->body->pretty_print_at(os, prec_let, false, false, spaces+4);
    }
}
