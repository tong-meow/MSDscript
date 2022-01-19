//
//  expr.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/17/22.
//

#include <iostream>
#include "expr.hpp"
#include "catch.hpp"

Num::Num(int val){
    this -> val = val;
}

bool Num::equals(Expr *e){
    Num *target = dynamic_cast<Num*>(e);
    if (target == NULL) return false;
    return ((this -> val) == (target -> val));
}


Add::Add(Expr* lhs, Expr* rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool Add::equals(Expr *e){
    Add *target = dynamic_cast<Add*>(e);
    if (target == NULL) return false;
    return (((this->lhs) -> equals (target->lhs))
            && ((this->rhs) -> equals (target->rhs)));
}


Mult::Mult(Expr* lhs, Expr* rhs){
    this -> lhs = lhs;
    this -> rhs = rhs;
}

bool Mult::equals(Expr *e){
    Mult *target = dynamic_cast<Mult*>(e);
    if (target == NULL) return false;
    return (((this->lhs) -> equals (target->lhs))
            && ((this->rhs) -> equals (target->rhs)));
}


Variable::Variable(std::string str){
    this -> str = str;
}

bool Variable::equals(Expr *e){
    Variable *target = dynamic_cast<Variable*>(e);
    if (target == NULL) return false;
    return ((this->str) == (target->str));
}


TEST_CASE("NumEquals"){
    CHECK((new Num(2))-> equals(new Num(2)) == true);
    CHECK((new Num(2))-> equals(new Num(5)) == false);
    CHECK((new Num(2))-> equals(new Add(new Num(2), new Num(5))) == false);
}

TEST_CASE("AddEquals"){
    Expr *test = new Add(new Num(2), new Num(5));
    CHECK(test-> equals(new Add(new Num(2), new Num(5))) == true);
    CHECK(test-> equals(new Add(new Num(5), new Num(2))) == false);
    CHECK(test-> equals(new Add(new Num(4), new Num(10))) == false);
    CHECK((new Num(2))-> equals(new Mult(new Num(3), new Num(6))) == false);
}

TEST_CASE("MultEquals"){
    Expr *test = new Mult(new Num(3), new Num(6));
    CHECK(test-> equals(new Mult(new Num(3), new Num(6))) == true);
    CHECK(test-> equals(new Mult(new Num(6), new Num(3))) == false);
    CHECK(test-> equals(new Mult(new Num(1), new Num(2))) == false);
    CHECK((new Num(2))-> equals(new Add(new Num(2), new Num(5))) == false);
}

TEST_CASE("VariableEquals"){
    Expr *test = new Variable("Happy2022");
    CHECK(test-> equals(new Variable("Happy2022")) == true);
    CHECK(test-> equals(new Variable("PlayStation")) == false);
    CHECK(test-> equals(new Variable("XBox")) == false);
    CHECK(test-> equals(new Mult(new Num(1), new Num(2))) == false);
}





