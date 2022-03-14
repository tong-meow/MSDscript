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


/*/////////////////////////////// NON-VIRTUAL FUNCTIONS ///////////////////////////////*/

// to_string function uses print() to return a string version of an Expr
std::string Expr::to_string(){
    std::stringstream ss;
    this -> print(ss);
    return ss.str();
}

// the pretty_print function uses helper preety_print_at() function to print an Expr
void Expr::pretty_print(std::ostream &os){
    this -> pretty_print_at(os, prec_none, false, false, 0);
}

// to_pretty_string function uses pretty_print() to return a string version of an Expr
std::string Expr::to_pretty_string(){
    std::stringstream ss;
    this -> pretty_print(ss);
    return ss.str();
}




/*/////////////////////////////// SUBCLASSES ///////////////////////////////*/

/*/////////////////// subclass 1: NumExpr ///////////////////*/

NumExpr::NumExpr(int rep){
    this -> rep = rep;
}

bool NumExpr::equals(Expr *e){
    NumExpr *target = dynamic_cast<NumExpr*>(e);
    if (target == NULL) return false;
    return ((this -> rep) == (target -> rep));
}

Val* NumExpr::interp(){
    return new NumVal(this -> rep);
}

bool NumExpr::has_variable(){
    return false;
}

Expr* NumExpr::subst(std::string s, Expr *e){
    return this;
}

void NumExpr::print(std::ostream& os){
    os << (this -> rep);
}

void NumExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                          bool nestedLet, int spaces){
    os << (this -> rep);
};




/*/////////////////// subclass 2: AddExpr ///////////////////*/

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
    if (prec < 3 || (prec==3 && !lhs)){
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



/*/////////////////// subclass 3: MultExpr ///////////////////*/

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
            mult_by ((this -> rhs) -> interp());
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
    if (prec >= 4 && lhs){
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



/*/////////////////// subclass 4: VarExpr ///////////////////*/

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
    throw std::runtime_error("Expr contains a string element.");
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





/*/////////////////// subclass 5: LetExpr ///////////////////*/
LetExpr::LetExpr(VarExpr* var, Expr* rhs, Expr* body){
    this -> var = var;
    this -> rhs = rhs;
    this -> body = body;
}

bool LetExpr::equals(Expr* e){
    LetExpr *target = dynamic_cast<LetExpr*>(e);
    if (target == NULL) return false;
    return ( (this->var) -> equals (target->var)
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
            -> subst ((this->var)->to_string(), rhs_val -> to_expr()))
            -> interp();
//    }
}

bool LetExpr::has_variable(){
    return ((this->rhs)->has_variable() || (this->body)->has_variable());
}

Expr* LetExpr::subst(std::string s, Expr *e){
    // compare string s with varName of the _let
    // if the names are same, use the nearest one
    if (s == (this->var)->to_string()){
        return new LetExpr(this->var,
                        this->rhs->subst(s,e),
                        this->body);
    }
    // if not, accumulate it
    else{
        return new LetExpr(this->var,
                        this->rhs->subst(s, e),
                        this->body->subst(s, e));
    }
}

void LetExpr::print(std::ostream& os){
    os << "(_let ";
    this->var->print(os);
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
        this->var->pretty_print_at(os, prec_let, false, false, spaces+1);
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
        this->var->pretty_print_at(os, prec_let, false, false, spaces);
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




/*/////////////////// subclass 6: BoolExpr ///////////////////*/
BoolExpr::BoolExpr(bool rep){
    this -> rep = rep;
}

bool BoolExpr::equals(Expr *e){
    BoolExpr *target = dynamic_cast<BoolExpr*>(e);
    if (target == NULL) return false;
    return (this -> rep) == (target -> rep);
}

Val* BoolExpr::interp(){
    return new BoolVal(this -> rep);
}

bool BoolExpr::has_variable(){
    return false;
}

Expr* BoolExpr::subst(std::string s, Expr *e){
    return this;
}

void BoolExpr::print(std::ostream& os){
    if (this -> rep){
        os << "_true";
    }else{
        os << "_false";
    }
}

void BoolExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                               bool nestedLet, int spaces){
    if (this -> rep){
        os << "_true";
    }else{
        os << "_false";
    }
}



/*/////////////////// subclass 7: EqualExpr ///////////////////*/

EqualExpr::EqualExpr(Expr* lhs, Expr* rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool EqualExpr::equals(Expr* e){
    EqualExpr *target = dynamic_cast<EqualExpr*>(e);
    if (target == NULL) return false;
    return ((this -> lhs) -> equals(target -> lhs)) &&
           ((this -> rhs) -> equals(target -> rhs));
}

Val* EqualExpr::interp(){
    if ((this -> lhs -> interp()) -> equals (this -> rhs -> interp()))
        return new BoolVal(true);
    else
        return new BoolVal(false);
}

bool EqualExpr::has_variable(){
    return ((this -> lhs) -> has_variable()) || ((this -> rhs) -> has_variable());
}

Expr* EqualExpr::subst(std::string s, Expr *e){
    return new EqualExpr(((this -> lhs) -> subst(s, e)), ((this -> rhs) -> subst(s, e)));
}

void EqualExpr::print(std::ostream& os){
    os << "(";
    (this -> lhs) -> print(os);
    os << " == ";
    (this -> rhs) -> print(os);
    os << ")";
}

void EqualExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                bool nestedLet, int spaces){
    long start = os.tellp();
    if (prec >= 2 && lhs){
        os << '(';
        long end1 = os.tellp();
        (this -> lhs) -> pretty_print_at(os, prec_equal, true, nestedLet, spaces+(int)(end1-start));
        os << " == ";
        long end2 = os.tellp();
        (this -> lhs) -> pretty_print_at(os, prec_equal, true, nestedLet, spaces+(int)(end2-start));
        os << ')';
    }else{
        (this -> lhs) -> pretty_print_at(os, prec_equal, true, nestedLet, spaces);
        os << " == ";
        long end3 = os.tellp();
        (this -> rhs) -> pretty_print_at(os, prec_equal, true, nestedLet, spaces+(int)(end3-start));
    }
}




/*/////////////////// subclass 8: IfExpr ///////////////////*/

IfExpr::IfExpr(Expr* test_part, Expr* then_part, Expr* else_part){
    this -> test_part = test_part;
    this -> then_part = then_part;
    this -> else_part = else_part;
}

bool IfExpr::equals(Expr* e){
    IfExpr *target = dynamic_cast<IfExpr*>(e);
    if (target == NULL) return false;
    return ((this -> test_part) -> equals(target -> test_part) &&
            (this -> then_part) -> equals(target -> then_part) &&
            (this -> else_part) -> equals(target -> else_part));
}

Val* IfExpr::interp(){
    // cast the test_part
    Val* test = this -> test_part -> interp();
    BoolVal *test_val = dynamic_cast<BoolVal*>(test);
    // null: test_part is not a boolean value, error
    if (test_val == NULL){
        throw std::runtime_error("Error: IfExpr's condition is not a boolean value.");
    }
    // true: value = then_part
    else if ( (test_val -> rep) == true ) {
        return this -> then_part -> interp();
    }
    // false: value = else_part
    else{
        return this -> else_part -> interp();
    }
}

bool IfExpr::has_variable(){
    return ((this -> test_part) -> has_variable() ||
            (this -> then_part) -> has_variable() ||
            (this -> else_part) -> has_variable());
}

Expr* IfExpr::subst(std::string s, Expr *e){
    return new IfExpr(((this -> test_part) -> subst(s, e)),
                      ((this -> then_part) -> subst(s, e)),
                      ((this -> else_part) -> subst(s, e)));
}

void IfExpr::print(std::ostream& os){
    os << "(_if (";
    this -> test_part -> print(os);
    os << ") _then (";
    this -> then_part -> print(os);
    os << ") _else (";
    this -> else_part -> print(os);
    os << "))";
}

void IfExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                     bool nestedLet, int spaces){
    os << "_if ";
    this -> test_part -> pretty_print_at(os, prec, lhs, nestedLet, spaces);
    os << "\n_then ";
    this -> then_part -> pretty_print_at(os, prec, lhs, nestedLet, spaces);
    os << "\n_else ";
    this -> else_part -> pretty_print_at(os, prec, lhs, nestedLet, spaces);
}




/*/////////////////// subclass 9: FunExpr ///////////////////*/

FunExpr::FunExpr (std::string formal_arg, Expr* body) {
    this->formal_arg = formal_arg;
    this->body = body;
}

bool FunExpr::equals(Expr *e) {
    FunExpr *target = dynamic_cast<FunExpr*>(e);
    if (target == NULL) return false;
    return (((this -> formal_arg) == (target -> formal_arg)) &&
            (this -> body) -> equals(target -> body));
}

Val * FunExpr::interp() {
    return new FunVal(formal_arg, body);
}

bool FunExpr::has_variable(){
    return body -> has_variable();
}

Expr* FunExpr::subst(std::string str, Expr* e) {
    if (formal_arg != str)
        return this;
    else
        return new FunExpr(formal_arg, body -> subst(str, e));
}

void FunExpr::print(std::ostream& os) {
    os << "(_fun (" << formal_arg << ") ";
    this -> body -> print(os);
    os << ")";
}

void FunExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                              bool nestedLet, int spaces) {
    // pretty-print needs modification
    os << "_fun (" << formal_arg << ")\n  ";
    body -> pretty_print_at(os, prec, lhs, nestedLet, spaces);
    os << "\n";
}



/*/////////////////// subclass 10: CallExpr ///////////////////*/

CallExpr::CallExpr (Expr *to_be_called, Expr *actual_arg) {
    this -> to_be_called = to_be_called;
    this -> actual_arg = actual_arg;
}

bool CallExpr::equals(Expr *e) {
    CallExpr* target = dynamic_cast<CallExpr*>(e);
    if (target == NULL) return false;
    return ((this -> to_be_called) -> equals(target -> to_be_called) &&
            (this -> actual_arg) -> equals(target->actual_arg));
}

Val * CallExpr::interp() {
    return to_be_called -> interp() -> call(actual_arg -> interp());
}

bool CallExpr::has_variable(){
    return (to_be_called -> has_variable() || actual_arg -> has_variable());
}

Expr* CallExpr::subst(std::string str, Expr* e) {
    return new CallExpr(to_be_called -> subst(str, e),
                        actual_arg -> subst(str, e));
}

void CallExpr::print(std::ostream& os) {
    this -> to_be_called->print(os);
    os << "(";
    this -> actual_arg->print(os);
    os << ")";
}

void CallExpr::pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                              bool nestedLet, int spaces) {
    // pretty-print needs modification
    this -> to_be_called -> pretty_print_at(os, prec, lhs, nestedLet, spaces);
    os << "(";
    this -> actual_arg -> pretty_print_at(os, prec, lhs, nestedLet, spaces);
    os << ")";
}
