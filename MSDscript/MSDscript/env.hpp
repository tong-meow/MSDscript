//
//  env.hpp
//  MSDscript
//
//  Created by Tong Shen on 3/17/22.
//

#ifndef env_hpp
#define env_hpp

#include <stdio.h>
#include <string>

#include "pointer.hpp"
#include "val.hpp"

class Val;

// template Env
class Env{
    
public:
    virtual bool equals(PTR(Env) env) = 0;
    virtual PTR(Val) lookup(std::string find_name) = 0;

};


// subclass1: empty env
class EmptyEnv : public Env {
    
public:
    EmptyEnv();
    bool equals(PTR(Env) env);
    PTR(Val) lookup(std::string find_name);
    
};


// subclass2: extended env
class ExtendedEnv : public Env {
    
public:
    std::string env_name;
    PTR(Val) env_val;
    PTR(Env) env_rest;
    
    ExtendedEnv(std::string env_name, PTR(Val) env_val, PTR(Env) env_rest);
    bool equals(PTR(Env) env);
    PTR(Val) lookup(std::string find_name);
    
};


#endif /* env_hpp */

