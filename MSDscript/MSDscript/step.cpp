//
//  step.cpp
//  MSDscript
//
//  Created by Tong Shen on 3/27/22.
//

#include "step.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "cont.hpp"

Step::mode_t Step::mode;
PTR(Cont) Step::cont;
PTR(Expr) Step::expr;   /* only for Step::interp_mode */
PTR(Env) Step::env;     /* only for Step::interp_mode */
PTR(Val) Step::val;     /* only for Step::continue_mode */


PTR(Val) Step::interp_by_steps(PTR(Expr) e) {
    Step::mode = Step::interp_mode;
    Step::expr = e;
    Step::env = NEW(EmptyEnv)();
    Step::val = nullptr;
    Step::cont = Cont::done;
    
    while (1) {
        if (Step::mode == Step::interp_mode) {
            Step::expr -> step_interp();
        }
        else {
            if (Step::cont == Cont::done) {
                return Step::val;
            } else {
                Step::cont -> step_continue();
            }
        }
    }
}

