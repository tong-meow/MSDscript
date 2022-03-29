//
//  cont.cpp
//  MSDscript
//
//  Created by Tong Shen on 3/27/22.
//

#include "cont.hpp"
#include "step.hpp"
#include "val.hpp"
#include "env.hpp"

PTR(Cont) Cont::done = NEW(DoneCont)();

/*////////////////// DONE CONT //////////////////*/

DoneCont::DoneCont() { }

void DoneCont::step_continue() {
    throw std::runtime_error("can't continue done");
}


/*////////////////// RIGHT THEN ADD CONT //////////////////*/


RightThenAddCont::RightThenAddCont(PTR(Expr) _rhs, PTR(Env) _env, PTR(Cont) _rest) {
    rhs = _rhs;
    env = _env;
    rest = _rest;
    
}

void RightThenAddCont::step_continue() {
    PTR(Val) lhs_val = Step::val;
    
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = Step::env;
    Step::cont = NEW(AddCont)(lhs_val, rest);
}


/*////////////////// ADD CONT //////////////////*/

AddCont::AddCont(PTR(Val) _lhs_val, PTR(Cont) _rest) {
    lhs_val = _lhs_val;
    rest = _rest;
}

void AddCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    
    Step::mode = Step::continue_mode;
    Step::val = lhs_val -> add_to(rhs_val);
    Step::cont = rest;
}



/*////////////////// RIGHT THEN MULT CONT //////////////////*/


RightThenMultCont::RightThenMultCont(PTR(Expr) _rhs, PTR(Env) _env, PTR(Cont) _rest) {
    rhs = _rhs;
    env = _env;
    rest = _rest;
    
}

void RightThenMultCont::step_continue() {
    PTR(Val) lhs_val = Step::val;
    
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = Step::env;
    Step::cont = NEW(MultCont)(lhs_val, rest);
}



/*////////////////// MULT CONT //////////////////*/

MultCont::MultCont(PTR(Val) _lhs_val, PTR(Cont) _rest) {
    lhs_val = _lhs_val;
    rest = _rest;
}

void MultCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    
    Step::mode = Step::continue_mode;
    Step::val = lhs_val -> mult_by(rhs_val);
    Step::cont = rest;
}



/*////////////////// LET BODY CONT //////////////////*/

LetBodyCont::LetBodyCont(std::string _varStr, PTR(Expr) _body, PTR(Env) _env, PTR(Cont) _rest) {
    varStr = _varStr;
    body = _body;
    env = _env;
    rest = _rest;
}

void LetBodyCont::step_continue() {
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(varStr, Step::val, env);
    
    Step::cont = rest;
}



/*////////////////// RIGHT THEN EQUAL CONT //////////////////*/

RightThenEqCont::RightThenEqCont(PTR(Expr) _rhs, PTR(Env) _env, PTR(Cont) _rest) {
    rhs = _rhs;
    env = _env;
    rest = _rest;
}


void RightThenEqCont::step_continue() {
    PTR(Val) lhs_val = Step::val;
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = env;
    
    Step::cont = NEW(EqualCont)(lhs_val, rest);
}



/*////////////////// EQUAL CONT //////////////////*/

EqualCont::EqualCont(PTR(Val) _lhs_val, PTR(Cont) _rest) {
    lhs_val = _lhs_val;
    rest = _rest;
}


void EqualCont::step_continue() {
    PTR(Val) rhs_val = Step::val;
    
    Step::mode = Step::continue_mode;
    if (lhs_val -> equals(rhs_val)) {
        Step::val = NEW(BoolVal)(true);
    } else {
        Step::val = NEW(BoolVal)(false);
    }
    
    Step::cont = rest;
    
}



/*////////////////// IF BRANCH CONT //////////////////*/

IfBranchCont::IfBranchCont(PTR(Expr) _then_part, PTR(Expr) _else_part, PTR(Env) _env, PTR(Cont) _rest) {
    then_part = _then_part;
    else_part = _else_part;
    env = _env;
    rest = _rest;
}


void IfBranchCont::step_continue() {
    PTR(Val) test_part = Step::val;
    Step::mode = Step::interp_mode;
    if (test_part -> is_true()) {
        Step::expr = then_part;
    } else {
        Step::expr = else_part;
    }
    Step::env = env;
    Step::cont = rest;
}



/*////////////////// ARG THEN CALL CONT //////////////////*/


ArgThenCallCont::ArgThenCallCont(PTR(Expr) _actual_arg, PTR(Env) _env, PTR(Cont) _rest) {
    actual_arg = _actual_arg;
    env = _env;
    rest = _rest;
}

void ArgThenCallCont::step_continue() {
    Step::mode = Step::interp_mode;
    Step::expr = actual_arg;
    Step::env = env;
    
    Step::cont = NEW(CallCont)(Step::val, rest);
}



/*////////////////// CALL CONT //////////////////*/



CallCont::CallCont(PTR(Val) _to_be_called_val, PTR(Cont) _rest) {
    to_be_called_val = _to_be_called_val;
    rest = _rest;
}

void CallCont::step_continue() {
    to_be_called_val -> call_step(Step::val, rest);
    
}
