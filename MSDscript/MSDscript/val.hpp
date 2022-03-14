//
//  val.hpp
//  MSDscript
//
//  Created by Tong Shen on 2/16/22.
//

#ifndef val_hpp
#define val_hpp

#include <stdio.h>
#include <string>

class Expr;

// template Val
class Val {
public:
    
    virtual bool equals(Val* v) = 0;
    virtual Expr* to_expr() = 0;
    virtual std::string to_string() = 0;
    virtual Val* add_to(Val* v) = 0;
    virtual Val* mult_by(Val* v) = 0;
    virtual Val* call(Val* arg) = 0;
    
};


// subclass 1: NumVal (number)
class NumVal : public Val {
public:
    int rep;
    
    NumVal(int rep);
    
    virtual bool equals(Val *v);
    virtual Expr* to_expr();
    virtual std::string to_string();
    virtual Val* add_to(Val* v);
    virtual Val* mult_by(Val* v);
    virtual Val* call(Val* arg);
};


// subclass 2: BoolVal (boolean)
class BoolVal : public Val {
public:
    bool rep;
    
    BoolVal(bool rep);
    
    virtual bool equals(Val *v);
    virtual Expr* to_expr();
    virtual std::string to_string();
    virtual Val* add_to(Val* v);
    virtual Val* mult_by(Val* v);
    virtual Val* call(Val* arg);
};


// subclass 3: FunVal (function)
class FunVal: public Val {
public:
    std::string formal_arg;
    Expr* body;

    FunVal(std::string formal_arg, Expr* body);
    
    virtual bool equals(Val *v);
    virtual Expr* to_expr();
    virtual std::string to_string();
    virtual Val* add_to(Val* v);
    virtual Val* mult_by(Val* v);
    virtual Val* call(Val* arg);
};


#endif /* val_hpp */
