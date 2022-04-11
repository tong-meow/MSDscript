//
//  val.hpp
//  MSDscript
//
//  Created by Tong Shen on 2/16/22.
//
#ifndef value_hpp
#define value_hpp

#include <stdio.h>
#include <string>
#include "pointer.hpp"

class Expr;
class Env;
class Cont;

// template Val
CLASS (Val) {
public:
    
    virtual bool equals(PTR(Val) v) = 0;
    virtual PTR(Expr) to_expr() = 0;
    virtual std::string to_string() = 0;
    virtual PTR(Val) add_to(PTR(Val) v) = 0;
    virtual PTR(Val) mult_by(PTR(Val) v) = 0;
    virtual PTR(Val) call(PTR(Val) arg) = 0;
    virtual bool is_true() = 0;
    virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) = 0;
    
};


// subclass 1: NumVal (number)
class NumVal : public Val {
public:
    int rep;
    
    NumVal(int val);
    
    virtual bool equals(PTR(Val) v);
    virtual PTR(Expr) to_expr();
    virtual std::string to_string();
    virtual PTR(Val) add_to(PTR(Val) v);
    virtual PTR(Val) mult_by(PTR(Val) v);
    virtual PTR(Val) call(PTR(Val) arg);
    virtual bool is_true();
    virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
};


// subclass 2: BoolVal (boolean)
class BoolVal : public Val {
public:
    bool rep;
    
    BoolVal(bool val);
    
    virtual bool equals(PTR(Val) v);
    virtual PTR(Expr) to_expr();
    virtual std::string to_string();
    virtual PTR(Val) add_to(PTR(Val) v);
    virtual PTR(Val) mult_by(PTR(Val) v);
    virtual PTR(Val) call(PTR(Val) arg);
    virtual bool is_true();
    virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
    
};


// subclass 3: FunVal (function)
class FunVal : public Val {
public:
    std::string formal_arg;
    PTR(Expr) body;
    PTR(Env) env;

    FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env);
    
    virtual bool equals(PTR(Val) v);
    virtual PTR(Expr) to_expr();
    virtual std::string to_string();
    virtual PTR(Val) add_to(PTR(Val) v);
    virtual PTR(Val) mult_by(PTR(Val) v);
    virtual PTR(Val) call(PTR(Val) arg);
    virtual bool is_true();
    virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
    
};


#endif /* val_hpp */
