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

// template Expr
class Expr {
public:
    // each subclass must override these functions
    virtual bool equals(Expr *e) = 0;
    virtual int interp() = 0;
    virtual bool has_variable() = 0;
    virtual Expr* subst(std::string s, Expr *e) = 0;
};


// subclass 1: Num
class Num : public Expr {
public:
    int val;
    
    Num(int val);
    virtual bool equals(Expr *e);
    virtual int interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
};


// subclass 2: Add
class Add : public Expr{
public:
    Expr* lhs;
    Expr* rhs;
    
    Add(Expr* lhs, Expr* rhs);
    virtual bool equals(Expr *e);
    virtual int interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
};


// subclass 3: Mult
class Mult : public Expr{
public:
    Expr* lhs;
    Expr* rhs;
    
    Mult(Expr* lhs, Expr* rhs);
    virtual bool equals(Expr *e);
    virtual int interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
};


// subclass 4: Variable
class Variable : public Expr{
public:
    std::string str;
    
    Variable(std::string str);
    virtual bool equals(Expr *e);
    virtual int interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
};


#endif /* expr_hpp */
