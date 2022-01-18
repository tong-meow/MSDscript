//
//  expr.hpp
//  hw2
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
    // each subclass must override this function
    virtual bool equals(Expr *e) = 0;
};


class Num : public Expr {
public:
    int val;
    
    Num(int val);
    virtual bool equals(Expr *e);
};


class Add : public Expr{
public:
    Expr* lhs;
    Expr* rhs;
    
    Add(Expr* lhs, Expr* rhs);
    virtual bool equals(Expr *e);
};


class Mult : public Expr{
public:
    Expr* lhs;
    Expr* rhs;
    
    Mult(Expr* lhs, Expr* rhs);
    virtual bool equals(Expr *e);
};


class Variable : public Expr{
public:
    std::string str;
    
    Variable(std::string str);
    virtual bool equals(Expr *e);
};

#endif /* expr_hpp */
