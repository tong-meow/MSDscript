//
//  env.cpp
//  MSDscript
//
//  Created by Tong Shen on 3/17/22.
//

#include "env.hpp"
#include "val.hpp"
#include "parse.hpp"

// subclass1: empty env
EmptyEnv::EmptyEnv(){}

PTR(Val) EmptyEnv::lookup(std::string find_name){
    throw std::runtime_error("Free variable: " + find_name);
}

bool EmptyEnv::equals(PTR(Env) env) {
    PTR(EmptyEnv) target_env = CAST(EmptyEnv)(env);
    if (target_env == NULL)
        throw std::runtime_error("Unable to cast the env to EmptyEnv");
    else
        return target_env;
}


// subclass2: extended env
ExtendedEnv::ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) env) {
    this -> env_name = name;
    this -> env_val = val;
    this -> env_rest = env;
}

PTR(Val) ExtendedEnv::lookup(std::string find_name){
    if (find_name == (this -> env_name))
        return env_val;
    else
        return (this -> env_rest) -> lookup(find_name);
}

bool ExtendedEnv::equals(PTR(Env) env){
    PTR(ExtendedEnv) target_env = CAST(ExtendedEnv)(env);
    if (target_env == NULL)
        return false;
    else
        return ((this -> env_name) == (target_env -> env_name) &&
                (this -> env_val) -> equals (target_env -> env_val) &&
                (this -> env_rest) -> equals (target_env -> env_rest));
}
