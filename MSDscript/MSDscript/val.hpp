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
    
};


// subclass 1: NumVal
class NumVal : public Val {
public:
    int val;
    
    NumVal(int val);
    
    virtual bool equals(Val *v);
    virtual Expr* to_expr();
    virtual std::string to_string();
    virtual Val* add_to(Val* v);
    virtual Val* mult_by(Val* v);
};

#endif /* val_hpp */
