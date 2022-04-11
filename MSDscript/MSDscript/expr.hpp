//
//  expr.hpp
//  MSDscript
//
//  Created by Tong Shen on 1/17/22.
//

#ifndef expr_hpp
#define expr_hpp

#include <stdio.h>
#include <string>
#include "pointer.hpp"

class Val;
class Env;



typedef enum {
    prec_none,            // = 0, NumExpr, VarExpr, BoolExpr
    prec_keywords,        // = 1, LetExpr, IfExpr, FunExpr, CallExpr
    prec_equal,           // = 2, EqualExpr
    prec_add,             // = 3, AddExpr
    prec_mult             // = 4, MultExpr
} precedence_t;


// template Expr
CLASS (Expr) {
public:
    // equals determines whether 2 Expr is equal or not
    virtual bool equals(PTR(Expr) e) = 0;
    // interp returns a integer value of an Expr
    virtual PTR(Val) interp(PTR(Env) env) = 0;
    
    // print is used to print the Expr to a referenced ostream
    virtual void print(std::ostream& os) = 0;
    // to_string is used to convert an Expr to its string version with print()
    std::string to_string();
    
    // pretty_print is used to print the Expr to a referenced ostream (with better layout)
    void pretty_print(std::ostream& os);
    // pretty_print_at is the helper of pretty_print()
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space) = 0;
    // to_pretty_string is used to convert an Expr to its string version with pretty_print()
    std::string to_pretty_string();
    
    virtual void step_interp() = 0;
};


// subclass 1: Num
class NumExpr : public Expr {
public:
    int rep;
    
    NumExpr(int rep);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};


// subclass 2: Add
class AddExpr : public Expr{
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};


// subclass 3: Mult
class MultExpr : public Expr{
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};


// subclass 4: Var
class VarExpr : public Expr{
public:
    std::string str;
    
    VarExpr(std::string str);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};


// subclass 5: _let
// 〈expr〉 = _let 〈variable〉 = 〈expr〉 _in 〈expr〉
class LetExpr : public Expr{
public:
    PTR(VarExpr) var;
    PTR(Expr) rhs;
    PTR(Expr) body;
    
    LetExpr(PTR(VarExpr) var, PTR(Expr) rhs, PTR(Expr) body);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};


// subclass 6: BoolExpr
class BoolExpr : public Expr{
public:
    bool rep;
    
    BoolExpr(bool rep);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};


// subclass 7: EqualExpr
class EqualExpr : public Expr{
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;
    
    EqualExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};


// subclass 8: IfExpr
class IfExpr : public Expr{
public:
    PTR(Expr) test_part;
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    
    IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};


// subclass 9: FunExpr
class FunExpr: public Expr {
public:
    std::string formal_arg;
    PTR(Expr) body;

    FunExpr (std::string formal_arg, PTR(Expr) body);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};


// subclass 10: CallExpr
class CallExpr: public Expr {
public:
    PTR(Expr) to_be_called;
    PTR(Expr) actual_arg;
    
    CallExpr (PTR(Expr) to_be_called, PTR(Expr) actual_arg);
    virtual bool equals(PTR(Expr) e);
    virtual PTR(Val) interp(PTR(Env) env);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t type, bool at_left, int space);
    void step_interp();
};

#endif /* expr_hpp */
