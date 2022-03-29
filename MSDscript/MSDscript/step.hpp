//
//  step.hpp
//  MSDscript
//
//  Created by Tong Shen on 3/27/22.
//

#ifndef step_hpp
#define step_hpp

#include <stdio.h>
#include "pointer.hpp"
#include "cont.hpp"

class Expr;
class Env;
class Val;

class Step {
    
public:
    
    typedef enum {
        interp_mode,
        continue_mode
    } mode_t;
    
    static mode_t mode;       /* choose mode */
    static PTR(Expr) expr;    /* for interp_mode */
    static PTR(Env) env;      /* for interp_mode */
    static PTR(Val) val;      /* for continue_mode */
    static PTR(Cont) cont;    /* for all modes */
    
    static PTR(Val) interp_by_steps(PTR(Expr) e);
    
};

#endif /* step_hpp */
