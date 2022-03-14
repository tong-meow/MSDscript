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

class Val;

typedef enum {
    prec_none,      // = 0
    prec_let,       // = 1
    prec_equal,        // = 2
    prec_add,       // = 3
    prec_mult       // = 4
} precedence_t;


// template Expr
class Expr {
public:
    // equals determines whether 2 Expr is equal or not
    virtual bool equals(Expr *e) = 0;
    // interp returns a integer value of an Expr
    virtual Val* interp() = 0;
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
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces) = 0;
    // to_pretty_string is used to convert an Expr to its string version with pretty_print()
    std::string to_pretty_string();
};


// subclass 1: Num
class NumExpr : public Expr {
public:
    int rep;
    
    NumExpr(int rep);
    virtual bool equals(Expr *e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
//    virtual void pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 2: Add
class AddExpr : public Expr{
public:
    Expr* lhs;
    Expr* rhs;
    
    AddExpr(Expr* lhs, Expr* rhs);
    virtual bool equals(Expr *e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
//    virtual void pretty_print_at(std::ostream& os, bool insideAdd, bool insideMult, bool lhs);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 3: Mult
class MultExpr : public Expr{
public:
    Expr* lhs;
    Expr* rhs;
    
    MultExpr(Expr* lhs, Expr* rhs);
    virtual bool equals(Expr *e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 4: Var
class VarExpr : public Expr{
public:
    std::string str;
    
    VarExpr(std::string str);
    virtual bool equals(Expr *e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 5: _let
// 〈expr〉 = _let 〈variable〉 = 〈expr〉 _in 〈expr〉
class LetExpr : public Expr{
public:
    VarExpr* var;
    Expr* rhs;
    Expr* body;
    
    LetExpr(VarExpr* var, Expr* rhs, Expr* body);
    virtual bool equals(Expr* e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 6: BoolExpr
class BoolExpr : public Expr{
public:
    bool rep;
    
    BoolExpr(bool rep);
    virtual bool equals(Expr* e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 7: EqualExpr
class EqualExpr : public Expr{
public:
    Expr* lhs;
    Expr* rhs;
    
    EqualExpr(Expr* lhs, Expr* rhs);
    virtual bool equals(Expr* e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 8: IfExpr
class IfExpr : public Expr{
public:
    Expr* test_part;
    Expr* then_part;
    Expr* else_part;
    
    IfExpr(Expr* test_part, Expr* then_part, Expr* else_part);
    virtual bool equals(Expr* e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 9: FunExpr
class FunExpr: public Expr {
public:
    std::string formal_arg;
    Expr* body;

    FunExpr (std::string formal_arg, Expr* body);
    virtual bool equals(Expr* e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};


// subclass 10: CallExpr
class CallExpr: public Expr {
public:
    Expr* to_be_called;
    Expr* actual_arg;
    
    CallExpr (Expr* to_be_called, Expr* actual_arg);
    virtual bool equals(Expr* e);
    virtual Val* interp();
    virtual bool has_variable();
    virtual Expr* subst(std::string s, Expr *e);
    virtual void print(std::ostream& os);
    virtual void pretty_print_at(std::ostream& os, precedence_t prec, bool lhs,
                                 bool nestedLet, int spaces);
};

#endif /* expr_hpp */
