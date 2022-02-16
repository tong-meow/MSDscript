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
    
    virtual bool equals();
    virtual Expr* to_expr();
    virtual std::string to_string();
    
};


// subclass 1: NumVal
class NumVal : public Val {
public:
    int val;
    
    virtual bool equals();
    virtual Expr* to_expr();
    virtual std::string to_string();
};

#endif /* val_hpp */
