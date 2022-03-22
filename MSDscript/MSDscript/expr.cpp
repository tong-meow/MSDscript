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


/*/////////////////////////////// NON-VIRTUAL FUNCTIONS ///////////////////////////////*/

// to_string function uses print() to return a string version of an Expr
std::string Expr::to_string(){
    std::stringstream ss;
    this -> print(ss);
    return ss.str();
}

// the pretty_print function uses helper preety_print_at() function to print an Expr
void Expr::pretty_print(std::ostream &os){
    this -> pretty_print_at(os, 0, 0);
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
    PTR(NumExpr) target = dynamic_cast<PTR(NumExpr)>(e);
    if (target == NULL) return false;
    return ((this -> rep) == (target -> rep));
}

PTR(Val) NumExpr::interp(PTR(Env) env){
    return new NumVal(this -> rep);
}

void NumExpr::print(std::ostream& os){
    os << (this -> rep);
}

void NumExpr::pretty_print(std::ostream &os){
    pretty_print_at(os, 0, 0);
}

void NumExpr::pretty_print_at(std::ostream& os, int level, int space){
    os << (this -> rep);
};




/*/////////////////// subclass 2: AddExpr ///////////////////*/

AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) e){
    PTR(AddExpr) target = dynamic_cast<PTR(AddExpr)>(e);
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

void AddExpr::pretty_print(std::ostream &os){
    (this -> lhs) -> pretty_print_at(os, 1, 0);
    os << " + ";
    (this -> rhs) -> pretty_print_at(os, 6, 0);
}

void AddExpr::pretty_print_at(std::ostream& os, int level, int space){
    if (level == 1 || level == 3 || level == 4){
        os << "(";
    }
    (this -> lhs) -> pretty_print_at(os, 3, space);
    os << " + ";
    (this -> rhs) -> pretty_print_at(os, 6, space);
    if (level == 1 || level == 3 || level == 4){
        os << ")";
    }
}



/*/////////////////// subclass 3: MultExpr ///////////////////*/

MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool MultExpr::equals(PTR(Expr) e){
    PTR(MultExpr) target = dynamic_cast<PTR(MultExpr)>(e);
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

void MultExpr::pretty_print(std::ostream &os){
    (this -> lhs) -> pretty_print_at(os, 4, 0);
    os << " * ";
    (this -> rhs) -> pretty_print_at(os, 1, 0);
}

void MultExpr::pretty_print_at(std::ostream& os, int level, int space){
    if (level == 3){
        (this -> lhs) -> pretty_print_at(os, 4, space);
        os << " * ";
        (this -> rhs) -> pretty_print_at(os, 3, space);
    } else {
        if (level == 4){
            os << "(";
        }
        (this -> lhs) -> pretty_print_at(os, 4, space);
        os << " * ";
        (this -> rhs) -> pretty_print_at(os, 1, space);
        if (level == 4){
            os << ")";
        }
    }
}



/*/////////////////// subclass 4: VarExpr ///////////////////*/

VarExpr::VarExpr(std::string str){
    this -> str = str;
}

bool VarExpr::equals(PTR(Expr) e){
    PTR(VarExpr) target = dynamic_cast<PTR(VarExpr)>(e);
    if (target == NULL) return false;
    return ((this -> str) == (target -> str));
}

PTR(Val) VarExpr::interp(PTR(Env) env){
    return env -> lookup(str);
}

void VarExpr::print(std::ostream& os){
    os << (this -> str);
}

void VarExpr::pretty_print(std::ostream &os){
    pretty_print_at(os, 0, 0);
}

void VarExpr::pretty_print_at(std::ostream& os, int level, int space){
    os << (this -> str);
};





/*/////////////////// subclass 5: LetExpr ///////////////////*/
LetExpr::LetExpr(PTR(VarExpr) var, PTR(Expr) rhs, PTR(Expr) body){
    this -> var = var;
    this -> rhs = rhs;
    this -> body = body;
}

bool LetExpr::equals(PTR(Expr) e){
    PTR(LetExpr) target = dynamic_cast<PTR(LetExpr)>(e);
    if (target == NULL) return false;
    return ( (this->var) -> equals (target->var)
            && (this->rhs) -> equals (target->rhs)
            && (this->body) -> equals (target->body)
            );
}

PTR(Val) LetExpr::interp(PTR(Env) env){
    PTR(Val) rhs_val = this -> rhs -> interp(env);
    PTR(Env) sub_env = new ExtendedEnv(var->str, rhs_val, env);
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

void LetExpr::pretty_print(std::ostream &os){
    os << "_let " << (this -> var) -> to_string() << " = ";
    (this -> rhs) -> pretty_print_at(os, 0, 0);
    os << "\n";
    int space = (int)os.tellp();
    os << "_in ";
    (this -> body) -> pretty_print_at(os, 0, space);
}

void LetExpr::pretty_print_at(std::ostream& os, int level, int space){
    if (level == 3 || level == 4 || level == 5){
        os << "(";
    }
    int current_position = (int)os.tellp();
    int spaces = current_position - space;
    os << "_let " << (this -> var) -> to_string() << " = ";
    (this -> rhs) -> pretty_print_at(os, 0, space);
    os << "\n";
    space = (int)os.tellp();
    int count = 0;
    while (count < spaces){
        os << " ";
        count++;
    }
    os << "_in ";
    (this -> body) -> pretty_print_at(os, 0, space);
    if (level == 3 || level == 4 || level == 5){
        os << ")";
    }
}




/*/////////////////// subclass 6: BoolExpr ///////////////////*/
BoolExpr::BoolExpr(bool rep){
    this -> rep = rep;
}

bool BoolExpr::equals(PTR(Expr) e){
    PTR(BoolExpr) target = dynamic_cast<PTR(BoolExpr)>(e);
    if (target == NULL) return false;
    return (this -> rep) == (target -> rep);
}

PTR(Val) BoolExpr::interp(PTR(Env) env){
    return new BoolVal(this -> rep);
}

void BoolExpr::print(std::ostream& os){
    if (this -> rep){
        os << "_true";
    }else{
        os << "_false";
    }
}

void BoolExpr::pretty_print(std::ostream &os){
    pretty_print_at(os, 0, 0);
}

void BoolExpr::pretty_print_at(std::ostream& os, int level, int space){
    if (this -> rep){
        os << "_true";
    }else{
        os << "_false";
    }
}



/*/////////////////// subclass 7: EqualExpr ///////////////////*/

EqualExpr::EqualExpr(PTR(Expr) lhs, PTR(Expr) rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool EqualExpr::equals(PTR(Expr) e){
    PTR(EqualExpr) target = dynamic_cast<PTR(EqualExpr)>(e);
    if (target == NULL) return false;
    return ((this -> lhs) -> equals(target -> lhs)) &&
           ((this -> rhs) -> equals(target -> rhs));
}

PTR(Val) EqualExpr::interp(PTR(Env) env){
    if ((this -> lhs -> interp(env)) -> equals (this -> rhs -> interp(env)))
        return new BoolVal(true);
    else
        return new BoolVal(false);
}

void EqualExpr::print(std::ostream& os){
    os << "(";
    (this -> lhs) -> print(os);
    os << " == ";
    (this -> rhs) -> print(os);
    os << ")";
}

void EqualExpr::pretty_print(std::ostream &os){
    (this -> lhs) -> pretty_print_at(os, 5, 0);
    os << " == ";
    (this -> rhs) -> pretty_print_at(os, 0, 0);
}

void EqualExpr::pretty_print_at(std::ostream& os, int level, int space){
    if (level == 0){
        (this -> lhs) -> pretty_print_at(os, 5, space);
        os << " == ";
        (this -> rhs) -> pretty_print_at(os, 0, space);
    } else {
        os << "(";
        (this -> lhs) -> pretty_print_at(os, 5, space);
        os << " == ";
        (this -> rhs) -> pretty_print_at(os, 0, space);
        os << ")";
    }
}




/*/////////////////// subclass 8: IfExpr ///////////////////*/

IfExpr::IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part){
    this -> test_part = test_part;
    this -> then_part = then_part;
    this -> else_part = else_part;
}

bool IfExpr::equals(PTR(Expr) e){
    PTR(IfExpr) target = dynamic_cast<PTR(IfExpr)>(e);
    if (target == NULL) return false;
    return ((this -> test_part) -> equals(target -> test_part) &&
            (this -> then_part) -> equals(target -> then_part) &&
            (this -> else_part) -> equals(target -> else_part));
}

PTR(Val) IfExpr::interp(PTR(Env) env){
    // cast the test_part
    PTR(Val) test = this -> test_part -> interp(env);
    PTR(BoolVal) test_val = dynamic_cast<PTR(BoolVal)>(test);
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

void IfExpr::pretty_print(std::ostream &os){
    os << "_if ";
    (this -> test_part) ->pretty_print_at(os, 0, 0);
    os << "\n";
    int space = (int)os.tellp();
    os << "_then ";
    (this -> then_part) ->pretty_print_at(os, 0, space);
    os << "\n";
    space = (int)os.tellp();
    os << "_else ";
    (this -> else_part) -> pretty_print_at(os, 0, space);
}

void IfExpr::pretty_print_at(std::ostream& os, int level, int space){
    if (level == 3 || level == 4 || level == 5){
        os << "(";
    }
    
    int current_position = (int)os.tellp();
    int spaces = current_position - space;
    
    os << "_if ";
    (this -> test_part) -> pretty_print_at(os, 0, space);
    os << "\n";
    
    space = (int)os.tellp();
    int count = 0;
    while (count < spaces){
        os << " ";
        count++;
    }
    os << "_then ";
    (this -> then_part) -> pretty_print_at(os, 0, space);
    os << "\n";
    
    space = (int)os.tellp();
    count = 0;
    while (count < spaces){
        os << " ";
        count++;
    }
    os << "_else ";
    (this -> else_part) -> pretty_print_at(os, 0, space);
    if (level == 3 || level == 4 || level == 5){
        os << ")";
    }
}




/*/////////////////// subclass 9: FunExpr ///////////////////*/

FunExpr::FunExpr (std::string formal_arg, PTR(Expr) body) {
    this -> formal_arg = formal_arg;
    this -> body = body;
}

bool FunExpr::equals(PTR(Expr) e) {
    PTR(FunExpr) target = dynamic_cast<PTR(FunExpr)>(e);
    if (target == NULL) return false;
    return (((this -> formal_arg) == (target -> formal_arg)) &&
            (this -> body) -> equals(target -> body));
}

/*
 FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env){
     this -> formal_arg = formal_arg;
     this -> body = body;
     this -> env = env;
 }
 */

PTR(Val) FunExpr::interp(PTR(Env) env) {
    return new FunVal(formal_arg, body, env);
}

void FunExpr::print(std::ostream& os) {
    os << "(_fun (" << formal_arg << ") ";
    this -> body -> print(os);
    os << ")";
}

void FunExpr::pretty_print(std::ostream &os){
    print(os);
}

void FunExpr::pretty_print_at(std::ostream& os, int level, int space) {
    print(os);
}



/*/////////////////// subclass 10: CallExpr ///////////////////*/

CallExpr::CallExpr (PTR(Expr) to_be_called, PTR(Expr) actual_arg) {
    this -> to_be_called = to_be_called;
    this -> actual_arg = actual_arg;
}

bool CallExpr::equals(PTR(Expr) e) {
    PTR(CallExpr) target = dynamic_cast<PTR(CallExpr)>(e);
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

void CallExpr::pretty_print(std::ostream &os){
    print(os);
}

void CallExpr::pretty_print_at(std::ostream& os, int level, int space) {
    print(os);
}
