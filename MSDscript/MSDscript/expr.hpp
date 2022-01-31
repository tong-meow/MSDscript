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


typedef enum {
    prec_none,      // = 0
    prec_let,       // = 1
    prec_add,       // = 2
    prec_mult       // = 3
} precedence_t;


// template Expr
class Expr {
public:
    // equals determines whether 2 Expr is equal or not
    virtual bool equals(Expr *e) = 0;
    // interp returns a integer value of an Expr
    virtual int interp() = 0;
    // has_variable determines if an Expr contains a string
    virtual bool has_variable() = 0;
    // substr is used to check if an Expr contains a target string s,
    // and if it does, change the target string to Expr e
    virtual Expr* subst(std::string s, Expr *e) = 0;
    
    // print is used to print the Expr to a referenced ostream
    virtual void print(std::ostream& os) = 0;
    // to_string is used to convert an Expr to its string version with print()
    std::string to_string();
    
    // pretty_print is used to print the Expr to a referenced ostream (with better layout)
    void pretty_print(std::ostream& os);
    // pretty_print_at is the helper of pretty_print()
//    virtual void pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs) = 0;
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces) = 0;
    // to_pretty_string is used to convert an Expr to its string version with pretty_print()
    std::string to_pretty_string();
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
    virtual void print(std::ostream& os);
//    virtual void pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
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
    virtual void print(std::ostream& os);
//    virtual void pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
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
    virtual void print(std::ostream& os);
//    virtual void pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 4: Var
class Var : public Expr{
public:
    std::string str;
    
    Var(std::string str);
    virtual bool equals(Expr *e);
    virtual int interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
//    virtual void pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 5: _let
// 〈expr〉 = _let 〈variable〉 = 〈expr〉 _in 〈expr〉
class _let : public Expr{
public:
    Var* varName;
    Expr* rhs;
    Expr* body;
    
    _let(Var* varName, Expr* rhs, Expr* body);
    virtual bool equals(Expr* e);
    virtual int interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
//    virtual void pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};

#endif /* expr_hpp */
