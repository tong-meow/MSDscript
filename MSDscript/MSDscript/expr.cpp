//
//  expr.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/17/22.
//


#include <sstream>
#include <string>
#include <stdexcept>
#include "expr.hpp"
#include "catch.hpp"
#include "parse.hpp"
#include "val.hpp"
#include "env.hpp"
#include "step.hpp"


/*/////////////////////////////// NON-VIRTUAL FUNCTIONS ///////////////////////////////*/

// to_string function uses print() to return a string version of an Expr
std::string Expr::to_string(){
    std::stringstream ss;
    this -> print(ss);
    return ss.str();
}

// the pretty_print function uses helper preety_print_at() function to print an Expr
void Expr::pretty_print(std::ostream &os){
    this -> pretty_print_at(os, prec_none, false, 0);
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

bool NumExpr::equals(PTR(Expr) e){
    PTR(NumExpr) target = CAST(NumExpr)(e);
    if (target == NULL) return false;
    return ((this -> rep) == (target -> rep));
}

PTR(Val) NumExpr::interp(PTR(Env) env){
    return NEW(NumVal)(rep);
}

void NumExpr::print(std::ostream& os){
    os << (this -> rep);
}

void NumExpr::pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space){
    os << (this -> rep);
};

void NumExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(NumVal)(rep);
}



/*/////////////////// subclass 2: AddExpr ///////////////////*/

AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) e){
    PTR(AddExpr) target = CAST(AddExpr)(e);
    if (target == NULL) return false;
    return (((this->lhs) -> equals (target->lhs))
            && ((this->rhs) -> equals (target->rhs)));
}

PTR(Val) AddExpr::interp(PTR(Env) env){
    return ((this -> lhs) -> interp(env))->
            add_to( (this -> rhs) -> interp(env));
}

void AddExpr::print(std::ostream& os){
    os << '(';
    (this->lhs) -> print(os);
    os << '+';
    (this->rhs) -> print(os);
    os << ')';
}

void AddExpr::pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space){
    /*
     When does AddExpr need ()?
     1. AddExpr is inside the MultExpr (both lhs and rhs)
     2. AddExpr is inside the AddExpr and is at lhs
     */
    if ((type >= prec_mult) || (type == prec_add && at_left)){
        os << "(";
    }
    (this -> lhs) -> pretty_print_at(os, prec_add, true, space);
    os << " + ";
    (this -> rhs) -> pretty_print_at(os, prec_add, false, space);
    if ((type >= prec_mult) || (type == prec_add && at_left)){
        os << ")";
    }
}

void AddExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}



/*/////////////////// subclass 3: MultExpr ///////////////////*/

MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool MultExpr::equals(PTR(Expr) e){
    PTR(MultExpr) target = CAST(MultExpr)(e);
    if (target == NULL) return false;
    return (((this->lhs) -> equals (target->lhs))
            && ((this->rhs) -> equals (target->rhs)));
}

PTR(Val) MultExpr::interp(PTR(Env) env){
    return ((this -> lhs) -> interp(env)) ->
            mult_by((this -> rhs) -> interp(env));
}

void MultExpr::print(std::ostream& os){
    os << '(';
    (this->lhs) -> print(os);
    os << '*';
    (this->rhs) -> print(os);
    os << ')';
}

void MultExpr::pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space){
    /*
     When does AddExpr need ()?
     1. MultExpr is inside MultExpr and is at lhs
     */
    if (type >= prec_mult && at_left){
        os << "(";
    }
    (this -> lhs) -> pretty_print_at(os, prec_mult, true, space);
    os << " * ";
    (this -> rhs) -> pretty_print_at(os, prec_mult, false, space);
    if (type >= prec_mult && at_left){
        os << ")";
    }
}

void MultExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);
}


/*/////////////////// subclass 4: VarExpr ///////////////////*/

VarExpr::VarExpr(std::string str){
    this -> str = str;
}

bool VarExpr::equals(PTR(Expr) e){
    PTR(VarExpr) target = CAST(VarExpr)(e);
    if (target == NULL) return false;
    return ((this -> str) == (target -> str));
}

PTR(Val) VarExpr::interp(PTR(Env) env){
    return env -> lookup(str);
}

void VarExpr::print(std::ostream& os){
    os << (this -> str);
}

void VarExpr::pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space){
    os << (this -> str);
};

void VarExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = Step::env -> lookup(str);
}




/*/////////////////// subclass 5: LetExpr ///////////////////*/
LetExpr::LetExpr(PTR(VarExpr) var, PTR(Expr) rhs, PTR(Expr) body){
    this -> var = var;
    this -> rhs = rhs;
    this -> body = body;
}

bool LetExpr::equals(PTR(Expr) e){
    PTR(LetExpr) target = CAST(LetExpr)(e);
    if (target == NULL) return false;
    return ( (this->var) -> equals (target->var)
            && (this->rhs) -> equals (target->rhs)
            && (this->body) -> equals (target->body)
            );
}

PTR(Val) LetExpr::interp(PTR(Env) env){
    PTR(Val) rhs_val = this -> rhs -> interp(env);
    PTR(Env) sub_env = NEW(ExtendedEnv)(var->str, rhs_val, env);
    return body -> interp(sub_env);
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

void LetExpr::pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space){
    /*
     When does LetExpr need ()?
     1. LetExpr is inside AddExpr and at its left side
     2. LetExpr is inside MultExpr and at its left side
     3. LetExpr is inside EqualExpr and at its left side
     */
    if (type >= prec_equal){
        os << "(";
    }
    
    os << "_let " << (this -> var) -> to_string() << " = ";
    (this -> rhs) -> pretty_print_at(os, prec_keywords, false, space+5);
    os << "\n";
    int count = 0;
    while (count < space){
        os << " ";
        count++;
    }
    os << "_in ";
    (this -> body) -> pretty_print_at(os, prec_keywords, false, space+4);
    
    if (type >= prec_equal){
        os << ")";
    }
}

void LetExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = Step::env;
    Step::cont = NEW(LetBodyCont)((this -> var) -> to_string(),
                                 (this -> body),
                                 Step::env,
                                 Step::cont);
}



/*/////////////////// subclass 6: BoolExpr ///////////////////*/
BoolExpr::BoolExpr(bool rep){
    this -> rep = rep;
}

bool BoolExpr::equals(PTR(Expr) e){
    PTR(BoolExpr) target = CAST(BoolExpr)(e);
    if (target == NULL) return false;
    return (this -> rep) == (target -> rep);
}

PTR(Val) BoolExpr::interp(PTR(Env) env){
    return NEW(BoolVal)(this -> rep);
}

void BoolExpr::print(std::ostream& os){
    if (this -> rep){
        os << "_true";
    }else{
        os << "_false";
    }
}

void BoolExpr::pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space){
    if (this -> rep){
        os << "_true";
    }else{
        os << "_false";
    }
}

void BoolExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(rep);
}



/*/////////////////// subclass 7: EqualExpr ///////////////////*/

EqualExpr::EqualExpr(PTR(Expr) lhs, PTR(Expr) rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool EqualExpr::equals(PTR(Expr) e){
    PTR(EqualExpr) target = CAST(EqualExpr)(e);
    if (target == NULL) return false;
    return ((this -> lhs) -> equals(target -> lhs)) &&
           ((this -> rhs) -> equals(target -> rhs));
}

PTR(Val) EqualExpr::interp(PTR(Env) env){
    if ((this -> lhs -> interp(env)) -> equals (this -> rhs -> interp(env)))
        return NEW(BoolVal)(true);
    else
        return NEW(BoolVal)(false);
}

void EqualExpr::print(std::ostream& os){
    os << "(";
    (this -> lhs) -> print(os);
    os << " == ";
    (this -> rhs) -> print(os);
    os << ")";
}


void EqualExpr::pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space){
    /*
     When does EqualExpr need ()?
     1. EqualExpr is inside AddExpr
     2. EqualExpr is inside MultExpr
     3. EqualExpr is inside EqualExpr and is lhs
     */
    if (type >= prec_add || (type == prec_equal && at_left)){
        os << "(";
    }
    (this -> lhs) -> pretty_print_at(os, prec_equal, true, space);
    os << " == ";
    (this -> rhs) -> pretty_print_at(os, prec_equal, false, space);
    if (type >= prec_add || (type == prec_equal && at_left)){
        os << ")";
    }
}

void EqualExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenEqCont)(rhs, Step::env, Step::cont);
}



/*/////////////////// subclass 8: IfExpr ///////////////////*/

IfExpr::IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part){
    this -> test_part = test_part;
    this -> then_part = then_part;
    this -> else_part = else_part;
}

bool IfExpr::equals(PTR(Expr) e){
    PTR(IfExpr) target = CAST(IfExpr)(e);
    if (target == NULL) return false;
    return ((this -> test_part) -> equals(target -> test_part) &&
            (this -> then_part) -> equals(target -> then_part) &&
            (this -> else_part) -> equals(target -> else_part));
}

PTR(Val) IfExpr::interp(PTR(Env) env){
    // cast the test_part
    PTR(Val) test = this -> test_part -> interp(env);
    PTR(BoolVal) test_val = CAST(BoolVal)(test);
    // null: test_part is not a boolean value, error
    if (test_val == NULL){
        throw std::runtime_error("IfExpr's condition is not a boolean value.");
    }
    // true: value = then_part
    else if ( (test_val -> rep) == true ) {
        return this -> then_part -> interp(env);
    }
    // false: value = else_part
    else{
        return this -> else_part -> interp(env);
    }
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

void IfExpr::pretty_print_at(std::ostream& os, precedence_t type, bool is_left, int space){
    /*
     When does IfExpr need ()?
     1. IfExpr is inside AddExpr and at its lhs
     2. IfExpr is inside MultExpr and at its lhs
     3. IfExpr is inside EqualExpr and at its lhs
     */
    if (type >= prec_equal && is_left){
        os << "(";
    }
    
    os << "_if ";
    (this -> test_part) -> pretty_print_at(os, prec_keywords, false, space+4);
    os << "\n";
    
    int count = 0;
    while (count < space){
        os << " ";
        count++;
    }
    os << "_then ";
    (this -> then_part) -> pretty_print_at(os, prec_keywords, false, space+6);
    os << "\n";
    
    count = 0;
    while (count < space){
        os << " ";
        count++;
    }
    os << "_else ";
    (this -> else_part) -> pretty_print_at(os, prec_keywords, false, space+6);
    
    if (type >= prec_equal && is_left){
        os << ")";
    }
}

void IfExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = test_part;
    Step::env = Step::env;
    Step::cont = NEW(IfBranchCont)(then_part, else_part, Step::env, Step::cont);
}



/*/////////////////// subclass 9: FunExpr ///////////////////*/

FunExpr::FunExpr (std::string formal_arg, PTR(Expr) body) {
    this -> formal_arg = formal_arg;
    this -> body = body;
}

bool FunExpr::equals(PTR(Expr) e) {
    PTR(FunExpr) target = CAST(FunExpr)(e);
    if (target == NULL) return false;
    return (((this -> formal_arg) == (target -> formal_arg)) &&
            (this -> body) -> equals(target -> body));
}

PTR(Val) FunExpr::interp(PTR(Env) env) {
    return NEW(FunVal)(formal_arg, body, env);
}

void FunExpr::print(std::ostream& os) {
    os << "(_fun (" << formal_arg << ") ";
    this -> body -> print(os);
    os << ")";
}

void FunExpr::pretty_print_at(std::ostream& os, precedence_t type, bool is_left, int space) {
    /*
     When does FunExpr need ()?
     1. FunExpr is inside AddExpr and at its lhs
     2. FunExpr is inside MultExpr and at its lhs
     3. FunExpr is inside EqualExpr and at its lhs
     */
    if (type >= prec_equal && is_left){
        os << "(";
    }
    
    os << "_fun (" << formal_arg << ") ";
    this -> body -> pretty_print_at(os, prec_keywords, false, space);
    
    if (type >= prec_equal && is_left){
        os << ")";
    }
}

void FunExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formal_arg, body, Step::env);
}


/*/////////////////// subclass 10: CallExpr ///////////////////*/

CallExpr::CallExpr (PTR(Expr) to_be_called, PTR(Expr) actual_arg) {
    this -> to_be_called = to_be_called;
    this -> actual_arg = actual_arg;
}

bool CallExpr::equals(PTR(Expr) e) {
    PTR(CallExpr) target = CAST(CallExpr)(e);
    if (target == NULL) return false;
    return ((this -> to_be_called) -> equals(target -> to_be_called) &&
            (this -> actual_arg) -> equals(target->actual_arg));
}

PTR(Val) CallExpr::interp(PTR(Env) env) {
    return to_be_called -> interp(env) -> call(actual_arg -> interp(env));
}

void CallExpr::print(std::ostream& os) {
    this -> to_be_called->print(os);
    os << "(";
    this -> actual_arg->print(os);
    os << ")";
}

void CallExpr::pretty_print_at(std::ostream& os, precedence_t type, bool is_left, int space) {
    /*
     When does CallExpr need ()?
     1. CallExpr is inside AddExpr and at its lhs
     2. CallExpr is inside MultExpr and at its lhs
     3. CallExpr is inside EqualExpr and at its lhs
     */
    if (type >= prec_equal && is_left){
        os << "(";
    }
    
    this -> to_be_called -> pretty_print_at(os, prec_keywords, false, space);
    os << " (";
    this -> actual_arg -> pretty_print_at(os, prec_keywords, false, space);
    os << ")";
    
    if (type >= prec_equal && is_left){
        os << ")";
    }
}

void CallExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::env = Step::env;
    Step::cont = NEW(ArgThenCallCont)(actual_arg, Step::env, Step::cont);
}
