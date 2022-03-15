//
//  tests.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/19/22.
//

#include <iostream>
#include <sstream>
#include "expr.hpp"
#include "parse.hpp"
#include "val.hpp"
#include "tests.hpp"
#include "pointer.hpp"

// EQUAL TESTS

TEST_CASE("Expr Equals"){
    
    SECTION("Num Equals"){
        CHECK((new NumExpr(2))-> equals(new NumExpr(2)) == true);
        CHECK((new NumExpr(2))-> equals(new NumExpr(5)) == false);
        CHECK((new NumExpr(2))-> equals(new AddExpr(new NumExpr(2), new NumExpr(5))) == false);
    }

    SECTION("Add Equals"){
        PTR(Expr) test = new AddExpr(new NumExpr(2), new NumExpr(5));
        CHECK(test-> equals(new AddExpr(new NumExpr(2), new NumExpr(5))) == true);
        CHECK(test-> equals(new AddExpr(new NumExpr(5), new NumExpr(2))) == false);
        CHECK(test-> equals(new AddExpr(new NumExpr(4), new NumExpr(10))) == false);
        CHECK(test-> equals(new MultExpr(new NumExpr(3), new NumExpr(6))) == false);
    }

    SECTION("Mult Equals"){
        PTR(Expr) test = new MultExpr(new NumExpr(3), new NumExpr(6));
        CHECK(test-> equals(new MultExpr(new NumExpr(3), new NumExpr(6))) == true);
        CHECK(test-> equals(new MultExpr(new NumExpr(6), new NumExpr(3))) == false);
        CHECK(test-> equals(new MultExpr(new NumExpr(1), new NumExpr(2))) == false);
        CHECK(test-> equals(new AddExpr(new NumExpr(2), new NumExpr(5))) == false);
    }
    
    SECTION("Var Equals"){
        PTR(Expr) test = new VarExpr("Happy2022");
        CHECK(test-> equals(new VarExpr("Happy2022")) == true);
        CHECK(test-> equals(new VarExpr("PlayStation")) == false);
        CHECK(test-> equals(new VarExpr("XBox")) == false);
        CHECK(test-> equals(new MultExpr(new NumExpr(1), new NumExpr(2))) == false);
    }

    SECTION("Let Equals"){
        // _let x = 5;
        // _in x + 1;
        PTR(LetExpr) test = new LetExpr(new VarExpr("x"), new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK(test-> equals(new LetExpr(new VarExpr("x"), new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1)))) == true);
        CHECK(test-> equals(new LetExpr(new VarExpr("y"), new NumExpr(5), new AddExpr(new VarExpr("y"), new NumExpr(1)))) == false);
        CHECK(test-> equals(new NumExpr(6)) == false);
    }

    SECTION("Bool Equals"){
        PTR(Expr) test = new BoolExpr(true);
        CHECK((test -> equals(new BoolExpr(true))) == true);
        CHECK((test -> equals(new BoolExpr(false))) == false);
        CHECK((test -> equals(new NumExpr(3))) == false);
    }

    SECTION("Equal Equals"){
        PTR(Expr) test = new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
        CHECK((test -> equals(new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2))))) == true);
        CHECK((test -> equals(new EqualExpr(new NumExpr(2), new NumExpr(3)))) == false);
        CHECK((test -> equals(new NumExpr(3))) == false);
    }

    SECTION("If Equals"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                new NumExpr(1), new NumExpr(0));
        CHECK((test -> equals(new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                         new NumExpr(1), new NumExpr(0)))) == true);
        CHECK((test -> equals(new NumExpr(3))) == false);
    }
    
    SECTION("Fun Equals"){
        // _fun (x) x + 2
        PTR(Expr) test = new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2)));
        CHECK(test -> equals(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2)))));
        CHECK(test -> equals(new FunExpr("x", new AddExpr(new NumExpr(2), new VarExpr("x")))) == false);
    }
    
    SECTION("Call Equals"){
        PTR(Expr) test = new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2))), new NumExpr(2));
        CHECK(test -> equals(new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2))),
                                          new NumExpr(2))));
        CHECK(test -> equals(new CallExpr(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2))),
                                          new NumExpr(-2)))
              == false);
    }
    
}


// INTERP TESTS

TEST_CASE("Expr Interp"){
    SECTION("Num Interp"){
        PTR(Expr) test = new NumExpr(42);
        CHECK( (test-> interp()) -> equals (new NumVal(42)));
    }

    SECTION("Add Interp"){
        // test == 1 + 2 + 3 == 6
        PTR(Expr) test = new AddExpr(new AddExpr(new NumExpr(1), new NumExpr(2)), new NumExpr(3));
        // test == 6
        CHECK( (test-> interp()) -> equals (new NumVal(6)));
        // throws exception
        CHECK_THROWS_WITH( (new AddExpr(new VarExpr("Wrong"), new NumExpr(1))) -> interp(),
                          "Expr contains a string element." );
    }

    SECTION("Mult Interp"){
        // test == 1 * 2 * 3 == 6
        PTR(Expr) test = new MultExpr(new MultExpr(new NumExpr(1), new NumExpr(2)), new NumExpr(3));
        // test == 6
        CHECK( (test-> interp()) -> equals (new NumVal(6)));
        // throws exception
        CHECK_THROWS_WITH( (new MultExpr(new VarExpr("Wrong"), new NumExpr(1))) -> interp(),
                          "Expr contains a string element." );
    }

    SECTION("Var Interp"){
        // test = "owls"
        PTR(Expr) test = new VarExpr("owls");
        // throws exception
        CHECK_THROWS_WITH(test -> interp(), "Expr contains a string element.");
    }

    SECTION("Let Interp"){
        // test1 =
        //        _let x = 5;
        //        _in 6;
        PTR(Expr) test1 = new LetExpr(new VarExpr("x"), new NumExpr(5), new NumExpr(6));
        CHECK( (test1 -> interp()) -> equals (new NumVal(6)));
        // test2 =
        //        _let x = 5;
        //        _in x + 1;
        PTR(Expr) test2 = new LetExpr(new VarExpr("x"),
                               new NumExpr(5),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK((test2 -> interp()) -> equals (new NumVal(6)));
        // test3 =
        //        _let x = 5;
        //        _in y + 1;
        PTR(Expr) test3 = new LetExpr(new VarExpr("x"),
                               new NumExpr(5),
                               new AddExpr(new VarExpr("y"), new NumExpr(1)));
        CHECK_THROWS_WITH( test3 -> interp(),
                          "Expr contains a string element." );
        // test4 =
        //        _let x = 5;
        //        _in _let x = 3;
        //            _in x + 1;
        PTR(Expr) test4 = new LetExpr(new VarExpr("x"),
                               new NumExpr(5),
                               new LetExpr(new VarExpr("x"),
                                        new NumExpr(3),
                                        new AddExpr(new VarExpr("x"), new NumExpr(1))));
        CHECK((test4 -> interp()) -> equals (new NumVal(4)));
        // test5 =
        //        _let x = 5;
        //        _in _let y = 3;
        //            _in x + 1;
        PTR(Expr) test5 = new LetExpr(new VarExpr("x"), new NumExpr(5),
                               new LetExpr(new VarExpr("y"),
                                        new NumExpr(3),
                                        new AddExpr(new VarExpr("x"), new NumExpr(1))));
        CHECK((test5 -> interp()) -> equals (new NumVal(6)));
        // test6 =
        //        _let x = 5;
        //        _in _let x = x + 1;
        //            _in x + 1;
        PTR(Expr) test6 = new LetExpr(new VarExpr("x"), new NumExpr(5),
                               new LetExpr(new VarExpr("x"),
                                        new AddExpr(new VarExpr("x"), new NumExpr(1)),
                                        new AddExpr(new VarExpr("x"), new NumExpr(1))));
        CHECK((test6 -> interp()) -> equals (new NumVal(7)));
        // test7 =
        //        _let x = x + 1;
        //        _in x + 1;
        PTR(Expr) test7 = new LetExpr(new VarExpr("x"),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK_THROWS_WITH( test7 -> interp(),
                          "Expr contains a string element." );
        // test8 =
        //        _let x = _let x = 3
        //                 _in x + 1
        //        _in x + 1
        PTR(Expr) test8 = new LetExpr(new VarExpr("x"),
                               new LetExpr(new VarExpr("x"), new NumExpr(3),
                                        new AddExpr(new VarExpr("x"), new NumExpr(1))),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK((test8 -> interp()) -> equals (new NumVal(5)));
    }

    SECTION("BoolExpr Interp"){
        PTR(Expr) test = new BoolExpr(false);
        CHECK(test -> interp() -> equals(new BoolVal(false)));
        CHECK(test -> interp() -> equals(new BoolVal(true)) == false);
    }

    SECTION("EqualExpr Interp"){
        PTR(Expr) test1 = new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
        CHECK((test1 -> interp()) -> equals(new BoolVal(true)) == true);
        CHECK((test1 -> interp()) -> equals(new BoolVal(false)) == false);
        PTR(Expr) test2 = new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(-2)));
        CHECK((test2 -> interp()) -> equals(new BoolVal(false)) == true);
        CHECK((test2 -> interp()) -> equals(new BoolVal(true)) == false);
    }

    SECTION("IfExpr Interp"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                new NumExpr(1), new NumExpr(0));
        CHECK((test1 -> interp()) -> equals(new NumVal(1)) == true);
        CHECK((test1 -> interp()) -> equals(new NumVal(0)) == false);
        // _if 1 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test2 = new IfExpr(new EqualExpr(new NumExpr(1), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                new NumExpr(1), new NumExpr(0));
        CHECK((test2 -> interp()) -> equals(new NumVal(1)) == false);
        CHECK((test2 -> interp()) -> equals(new NumVal(0)) == true);
        // if 4
        // _then 1
        // _else 0
        PTR(Expr) test3 = new IfExpr(new NumExpr(4),new NumExpr(1), new NumExpr(0));
        CHECK_THROWS_WITH(test3 -> interp(), "Error: IfExpr's condition is not a boolean value.");
    }
    
    SECTION("FunExpr Interp"){
        PTR(Expr) test = new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2)));
        CHECK((test -> interp()) -> equals(new FunVal("x", new AddExpr(new VarExpr("x"), new NumExpr(2)))));
        CHECK((test -> interp()) -> equals(new FunVal("y", new AddExpr(new VarExpr("y"), new NumExpr(2)))) == false);
    }
    
    SECTION("CallExpr Interp"){
        PTR(Expr) fun = new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2)));
        PTR(Expr) test = new CallExpr(fun, new NumExpr(2));
        CHECK((test -> interp()) -> equals((fun -> interp()) -> call((new NumExpr(2)) -> interp())));
    }
}

// HAS_VARIABLE TESTS

TEST_CASE("Expr Has Variable"){
    SECTION("Num Has Variable"){
        CHECK((new NumExpr(2)) -> has_variable() == false);
    }

    SECTION("Add Has Variable"){
        PTR(Expr) test1 = new AddExpr(new AddExpr(new NumExpr(1), new NumExpr(1)), new NumExpr(1));
        CHECK((test1 -> has_variable()) == false);
        PTR(Expr) test2 = new AddExpr(new AddExpr(new NumExpr(1), new VarExpr("owls")), new NumExpr(1));
        CHECK((test2 -> has_variable()) == true);
    }

    SECTION("Mult Has Variable"){
        PTR(Expr) test1 = new MultExpr(new AddExpr(new NumExpr(1), new NumExpr(1)), new NumExpr(1));
        CHECK((test1 -> has_variable()) == false);
        PTR(Expr) test2 = new MultExpr(new MultExpr(new NumExpr(1), new VarExpr("owls")), new NumExpr(1));
        CHECK((test2 -> has_variable()) == true);
    }

    SECTION("Variable Has Variable"){
        CHECK((new VarExpr("owls")) -> has_variable() == true);
    }

    SECTION("Let Has Variable"){
        // test1 =
        //          _let x = 5;
        //          _in 6;
        PTR(Expr) test1 = new LetExpr(new VarExpr("x"), new NumExpr(5), new NumExpr(6));
        CHECK(test1 -> has_variable() == false);
        // test2 =
        //          _let x = x + 1;
        //          _in 6;
        PTR(Expr) test2 = new LetExpr(new VarExpr("x"),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)),
                               new NumExpr(6));
        CHECK(test2 -> has_variable() == true);
        // test3 =
        //          _let x = 5;
        //          _in x + 1;
        PTR(Expr) test3 = new LetExpr(new VarExpr("x"),
                               new NumExpr(5),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK(test3 -> has_variable() == true);
        // test4 =
        //          _let x = x + 1;
        //          _in x + 1;
        PTR(Expr) test4 = new LetExpr(new VarExpr("x"),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK(test4 -> has_variable() == true);
    }

    SECTION("BoolExpr Has Variable"){
        PTR(Expr) test = new BoolExpr(true);
        CHECK(test -> has_variable() == false);
    }

    SECTION("EqualExpr Has Variable"){
        PTR(Expr) test1 = new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
        CHECK(test1 -> has_variable() == false);
        PTR(Expr) test2 = new EqualExpr(new NumExpr(3), new AddExpr(new VarExpr("1"), new NumExpr(2)));
        CHECK(test2 -> has_variable() == true);
    }

    SECTION("IfExpr Has Variable"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                new NumExpr(1), new NumExpr(0));
        CHECK((test1 -> has_variable()) == false);
        // _if 1 == -1 + 1
        // _then good
        // _else bad
        PTR(Expr) test2 = new IfExpr(new EqualExpr(new NumExpr(1), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                new VarExpr("good"), new VarExpr("bad"));
        CHECK((test2 -> has_variable()) == true);
    }
    
    SECTION("FunExpr Has Variable"){
        PTR(Expr) test1 = new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2)));
        CHECK(test1 -> has_variable() == true);
        
        PTR(Expr) test2 = new FunExpr("x", new AddExpr(new NumExpr(1), new NumExpr(2)));
        CHECK(test2 -> has_variable() == false);
    }
    
    SECTION("CallExpr Has Variable"){
        PTR(Expr) fun1 = new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2)));
        PTR(Expr) test1 = new CallExpr(fun1, new NumExpr(2));
        CHECK(test1 -> has_variable() == true);
        
        PTR(Expr) fun2 = new FunExpr("x", new AddExpr(new NumExpr(-2), new NumExpr(2)));
        PTR(Expr) test2 = new CallExpr(fun2, new NumExpr(2));
        CHECK(test2 -> has_variable() == false);
    }
}


// SUBST TESTS

TEST_CASE("Expr Subst"){
    SECTION("Num Subst"){
        CHECK(((new NumExpr(2))->subst("owls", new VarExpr("snowy owls"))) ->equals((new NumExpr(2))));
        CHECK(((new NumExpr(2))->subst("owls", new NumExpr(3))) ->equals((new NumExpr(2))));
    }

    SECTION("Add Subst"){
        // test1 contains no string
        PTR(Expr) test1 = new AddExpr(new NumExpr(3), new MultExpr(new NumExpr(1), new NumExpr(5)));
        CHECK(((test1 -> subst("owls", new VarExpr("snowy owls"))) ->equals(test1)));
        
        // test2 contains target string
        PTR(Expr) test2 = new AddExpr(new NumExpr(3), new MultExpr(new VarExpr("owls"), new NumExpr(5)));
        CHECK(((test2 -> subst("owls", new VarExpr("snowy owls"))) ->equals(
           new AddExpr(new NumExpr(3), new MultExpr(new VarExpr("snowy owls"), new NumExpr(5))))));
        
        // test3 contains string but != target
        PTR(Expr) test3 = new AddExpr(new NumExpr(3), new MultExpr(new VarExpr("barn owls"), new NumExpr(5)));
        CHECK(((test3 -> subst("owls", new VarExpr("snowy owls"))) -> equals(test3)));
    }

    SECTION("Mult Subst"){
        // test1 contains no string
        PTR(Expr) test1 = new MultExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(5)));
        CHECK(((test1 -> subst("owls", new VarExpr("snowy owls"))) ->equals(test1)));
        
        // test2 contains target string
        PTR(Expr) test2 = new MultExpr(new NumExpr(3), new AddExpr(new VarExpr("owls"), new NumExpr(5)));
        CHECK(((test2 -> subst("owls", new VarExpr("snowy owls"))) ->equals(
           new MultExpr(new NumExpr(3), new AddExpr(new VarExpr("snowy owls"), new NumExpr(5))))));
        
        // test3 contains string but != target
        PTR(Expr) test3 = new MultExpr(new NumExpr(3), new MultExpr(new VarExpr("barn owls"), new NumExpr(5)));
        CHECK(((test3 -> subst("owls", new VarExpr("snowy owls"))) -> equals(test3)));
    }

    SECTION("Var Subst"){
        CHECK(((new VarExpr("owls"))->subst("owls", new VarExpr("snowy owls")))
              ->equals((new VarExpr("snowy owls"))));
        CHECK(((new VarExpr("barn owls"))->subst("owls", new VarExpr("snowy owls")))
              ->equals((new VarExpr("barn owls"))));
    }

    SECTION("Let Subst"){
        // test1 =
        //          _let x = 5;
        //          _in 6;          -> subst ("x", 1)
        PTR(Expr) test1 = new LetExpr(new VarExpr("x"), new NumExpr(5), new NumExpr(6));
        CHECK((test1 -> subst("x", new NumExpr(1))) -> equals(test1));
        // test2 =
        //          _let x = x + 1;
        //          _in x + 1;          -> subst ("x", 5)
        PTR(Expr) test2 = new LetExpr(new VarExpr("x"),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK(test2 -> subst("x", new NumExpr(5)) -> equals(
              new LetExpr(new VarExpr("x"),
                       new AddExpr(new NumExpr(5), new NumExpr(1)),
                       new AddExpr(new VarExpr("x"), new NumExpr(1)))));
        // test3 =
        //          _let x = x + 1;
        //          _in x + 1;          -> subst ("y", 5)
        PTR(Expr) test3 = new LetExpr(new VarExpr("x"),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK(test3 -> subst("y", new NumExpr(5)) -> equals(test3));
    }

    SECTION("BoolExpr Subst"){
        CHECK(((new BoolExpr(true))->subst("owls", new VarExpr("snowy owls"))) ->equals((new BoolExpr(true))));
        CHECK(((new BoolExpr(false))->subst("owls", new NumExpr(3))) ->equals((new BoolExpr(false))));
    }

    SECTION("EqualExpr Subst"){
        PTR(Expr) test = new EqualExpr(new VarExpr("grade"), new VarExpr("finalGPA"));
        CHECK((test -> subst("finalGPA", new NumExpr(4))) -> equals
              (new EqualExpr(new VarExpr("grade"), new NumExpr(4))) == true);
        CHECK((test -> subst("finalGPA", new NumExpr(4))) -> equals
              (new EqualExpr(new NumExpr(4), new VarExpr("finalGPA"))) == false);
    }

    SECTION("IfExpr Subst"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                new NumExpr(1), new NumExpr(0));
        CHECK((test1 -> subst("good", new NumExpr(42))) -> equals(test1) == true);
        // _if 0 == -1 + 1
        // _then good
        // _else 0
        PTR(Expr) test2 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                new VarExpr("good"), new NumExpr(0));
        CHECK((test2 -> subst("good", new NumExpr(1))) -> equals(test1) == true);
    }
    
    SECTION("FunExpr Subst"){
        PTR(Expr) test = new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2)));
        CHECK((test -> subst("x", new NumExpr(3))) -> equals (new FunExpr("x",
                                                                          new AddExpr(new NumExpr(3), new NumExpr(2)))));
        CHECK((test -> subst("y", new NumExpr(3))) -> equals (test));
    }
    
    SECTION("CallExpr Subst"){
        PTR(Expr) fun = new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2)));
        PTR(Expr) test = new CallExpr(fun, new NumExpr(2));
        CHECK(test -> subst("x", new NumExpr(3)) -> equals
              (new CallExpr(new FunExpr("x",
                                        new AddExpr(new NumExpr(3), new NumExpr(2))),
                            new NumExpr(2))));
        CHECK(test -> subst("y", new NumExpr(3)) -> equals (test));
    }
}

// PRINT TESTS

TEST_CASE("Expr Print"){
    SECTION("Num Print"){
        PTR(Expr) test1 = new NumExpr(10);
        CHECK((test1 -> to_string()) == "10");
    }

    SECTION("Add Print"){
        PTR(Expr) test1 = new AddExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
        CHECK((test1 -> to_string()) == "(3+(1+2))");
        PTR(Expr) test2 = new AddExpr(new AddExpr(new NumExpr(3), new NumExpr(1)), new NumExpr(2));
        CHECK((test2 -> to_string()) == "((3+1)+2)");
    }

    SECTION("Mult Print"){
        PTR(Expr) test1 = new MultExpr(new NumExpr(3), new MultExpr(new NumExpr(1), new NumExpr(2)));
        CHECK((test1 -> to_string()) == "(3*(1*2))");
        PTR(Expr) test2 = new MultExpr(new MultExpr(new NumExpr(3), new NumExpr(1)), new NumExpr(2));
        CHECK((test2 -> to_string()) == "((3*1)*2)");
    }

    SECTION("Variable Print"){
        PTR(Expr) test1 = new VarExpr("X");
        CHECK((test1 -> to_string()) == "X");
    }

    SECTION("Let Print"){
        // test1 =
        //        _let x = 5;
        //        _in x + 1;
        PTR(Expr) test1 = new LetExpr(new VarExpr("x"),
                               new NumExpr(5),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK((test1 -> to_string()) == "(_let x=5 _in (x+1))");
        // test2 =
        //        _let x = 5;
        //        _in _let x = 3;
        //            _in x + 1;
        PTR(Expr) test2 = new LetExpr(new VarExpr("x"),
                               new NumExpr(5),
                               new LetExpr(new VarExpr("x"),
                                        new NumExpr(3),
                                        new AddExpr(new VarExpr("x"), new NumExpr(1))));
        CHECK((test2 -> to_string()) == "(_let x=5 _in (_let x=3 _in (x+1)))");
        // test3 =
        //        _let x = x + 1;
        //        _in x + 1;
        PTR(Expr) test3 = new LetExpr(new VarExpr("x"),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK((test3 -> to_string()) == "(_let x=(x+1) _in (x+1))");
    }

    SECTION("boolExpr Print"){
        PTR(Expr) test1 = new BoolExpr(true);
        CHECK((test1 -> to_string()) == "_true");
        PTR(Expr) test2 = new BoolExpr(false);
        CHECK((test2 -> to_string()) == "_false");
    }

    SECTION("EqualExpr Print"){
        PTR(Expr) test1 = new EqualExpr(new VarExpr("grade"), new NumExpr(99));
        CHECK((test1 -> to_string()) == "(grade == 99)");
        PTR(Expr) test2 = new EqualExpr(new VarExpr("grade"), new AddExpr(new NumExpr(30), new NumExpr(60)));
        CHECK((test2 -> to_string()) == "(grade == (30+60))");
    }

    SECTION("IfExpr Print"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                new NumExpr(1), new NumExpr(0));
        CHECK((test1 -> to_string()) == "(_if ((0 == (-1+1))) _then (1) _else (0))");
        // if good
        // _then 1
        // _else 0
        PTR(Expr) test2 = new IfExpr(new VarExpr("good"),new NumExpr(1), new NumExpr(0));
        CHECK((test2 -> to_string()) == "(_if (good) _then (1) _else (0))");
    }
    
    SECTION("FunExpr Print"){
        PTR(Expr) test = new FunExpr("testFun", new AddExpr(new VarExpr("x"), new NumExpr(2)));
        CHECK(test -> to_string() == "(_fun (testFun) (x+2))");
    }
    
    SECTION("CallExpr Print"){
        PTR(Expr) fun = new FunExpr("testFun", new AddExpr(new VarExpr("x"), new NumExpr(2)));
        PTR(Expr) test = new CallExpr(fun, new NumExpr(2));
        CHECK(test -> to_string() == "(_fun (testFun) (x+2))(2)");
    }
}

// PRETTY_PRINT TESTS

TEST_CASE("Expr Pretty Print"){
    SECTION("Num Pretty Print"){
        PTR(Expr) test1 = new NumExpr(10);
        CHECK((test1 -> to_pretty_string()) == "10");
    }

    SECTION("Add Pretty Print"){
        PTR(Expr) test1 = new AddExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
        CHECK((test1 -> to_pretty_string()) == "3 + 1 + 2");
        PTR(Expr) test2 = new AddExpr(new AddExpr(new NumExpr(3), new NumExpr(1)), new NumExpr(2));
        CHECK((test2 -> to_pretty_string()) == "(3 + 1) + 2");
    }

    SECTION("Mult Pretty Print"){
        PTR(Expr) test1 = new MultExpr(new NumExpr(3), new MultExpr(new NumExpr(1), new NumExpr(2)));
        CHECK((test1 -> to_pretty_string()) == "3 * 1 * 2");
        PTR(Expr) test2 = new MultExpr(new MultExpr(new NumExpr(3), new NumExpr(1)), new NumExpr(2));
        CHECK((test2 -> to_pretty_string()) == "(3 * 1) * 2");
    }

    SECTION("Add and Mult Mixed Pretty Print"){
        // 1 + 2 * 3
        PTR(Expr) test1 = new AddExpr(new NumExpr(1), new MultExpr(new NumExpr(2), new NumExpr(3)));
        CHECK((test1 -> to_pretty_string()) == "1 + 2 * 3");
        // 1 * (2 + 3)
        PTR(Expr) test2 = new MultExpr(new NumExpr(1), new AddExpr(new NumExpr(2), new NumExpr(3)));
        CHECK((test2 -> to_pretty_string()) == "1 * (2 + 3)");
        // 1 * (2 + 3) * 4
        PTR(Expr) test3 = new MultExpr(new NumExpr(1), new MultExpr(new AddExpr(new NumExpr(2), new NumExpr(3)), new NumExpr(4)));
        CHECK((test3 -> to_pretty_string()) == "1 * (2 + 3) * 4");
        // (1 + 2) * (3 + 4)
        PTR(Expr) test4 = new MultExpr(new AddExpr(new NumExpr(1), new NumExpr(2)), new AddExpr(new NumExpr(3), new NumExpr(4)));
        CHECK((test4 -> to_pretty_string()) == "(1 + 2) * (3 + 4)");
        // (1 * (2 + 3)) * 4
        PTR(Expr) test5 = new MultExpr(new MultExpr(new NumExpr(1), new AddExpr(new NumExpr(2), new NumExpr(3))), new NumExpr(4));
        CHECK((test5 -> to_pretty_string()) == "(1 * (2 + 3)) * 4");
    }

    SECTION("Var Pretty Print"){
        PTR(Expr) test1 = new VarExpr("Y");
        CHECK((test1 -> to_pretty_string()) == "Y");
    }

    SECTION("_let Pretty Print"){
        // test1 =
        //        _let x = 5
        //        _in x + 1
        PTR(Expr) test1 = new LetExpr(new VarExpr("x"),
                               new NumExpr(5),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK((test1 -> to_pretty_string()) == "_let x = 5\n_in x + 1");
        // test2 =
        //        (_let x = 5
        //        _in x) + 1
        PTR(Expr) test2 = new AddExpr(new LetExpr(new VarExpr("x"), new NumExpr(5), new VarExpr("x")),
                              new NumExpr(1));
        CHECK((test2 -> to_pretty_string()) == "(_let x = 5\n _in x) + 1");
        // test3 =
        //        5 * (_let x = 5
        //             _in  x) + 1
        PTR(Expr) test3 = new AddExpr (new MultExpr(new NumExpr(5),
                                        new LetExpr(new VarExpr("x"), new NumExpr(5), new VarExpr("x"))),
                               new NumExpr(1));
        CHECK((test3 -> to_pretty_string()) == "5 * (_let x = 5\n"
                                               "     _in x) + 1");
        // test4 =
        //        5 * _let x = 5
        //            _in  x + 1
        PTR(Expr) test4 = new MultExpr(new NumExpr(5),
                               new LetExpr(new VarExpr("x"),
                                        new NumExpr(5),
                                        new AddExpr(new VarExpr("x"), new NumExpr(1))));
        CHECK((test4 -> to_pretty_string()) == "5 * _let x = 5\n"
                                               "    _in x + 1");
        // test5 =
        //        _let x = 5
        //        _in _let x = 3
        //            _in x + 1
        PTR(Expr) test5 = new LetExpr(new VarExpr("x"),
                               new NumExpr(5),
                               new LetExpr(new VarExpr("x"), new NumExpr(3),
                                       new AddExpr(new VarExpr("x"), new NumExpr(1))));
        CHECK((test5 -> to_pretty_string()) == "_let x = 5\n"
                                               "_in _let x = 3\n"
                                               "    _in x + 1");
        // test6 =
        //        _let x = _let x = 3
        //                 _in x + 1
        //        _in x + 1
        PTR(Expr) test6 = new LetExpr(new VarExpr("x"),
                               new LetExpr(new VarExpr("x"), new NumExpr(3),
                                        new AddExpr(new VarExpr("x"), new NumExpr(1))),
                               new AddExpr(new VarExpr("x"), new NumExpr(1)));
        CHECK((test6 -> to_pretty_string()) == "_let x = _let x = 3\n"
                                               "         _in x + 1\n"
                                               "_in x + 1");
        // test7 =
        //       (5 + (_let x = _let x = 1
        //                      _in x + 2
        //             _in x + 3)) * 4
        PTR(Expr) test7 = new MultExpr(new AddExpr(new NumExpr(5),
                                       new LetExpr(new VarExpr("x"),
                                                new LetExpr(new VarExpr("x"),
                                                         new NumExpr(1),
                                                         new AddExpr(new VarExpr("x"), new NumExpr(2))),
                                                new AddExpr(new VarExpr("x"), new NumExpr(3)))),
                               new NumExpr(4));
        CHECK((test7 -> to_pretty_string()) == "(5 + (_let x = _let x = 1\n"
                                               "               _in x + 2\n"
                                               "      _in x + 3)) * 4");
    }

    SECTION("boolExpr Pretty Print"){
        PTR(Expr) test1 = new BoolExpr(true);
        CHECK((test1 -> to_pretty_string()) == "_true");
        PTR(Expr) test2 = new BoolExpr(false);
        CHECK((test2 -> to_pretty_string()) == "_false");
    }

    SECTION("EqualExpr Pretty Print"){
        PTR(Expr) test1 = new EqualExpr(new VarExpr("grade"), new NumExpr(99));
        CHECK((test1 -> to_pretty_string()) == "grade == 99");
        PTR(Expr) test2 = new EqualExpr(new VarExpr("grade"), new AddExpr(new NumExpr(10), new NumExpr(80)));
        CHECK((test2 -> to_pretty_string()) == "grade == 10 + 80");
    }

    SECTION("IfExpr Pretty Print"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                new NumExpr(1), new NumExpr(0));
        CHECK((test1 -> to_pretty_string()) == "_if 0 == -1 + 1\n"
                                               "_then 1\n"
                                               "_else 0");
        // if good
        // _then 1
        // _else 0
        PTR(Expr) test2 = new IfExpr(new VarExpr("good"),new NumExpr(1), new NumExpr(0));
        CHECK((test2 -> to_pretty_string()) == "_if good\n"
                                               "_then 1\n"
                                               "_else 0");
    }
}



// PARSE TESTS

TEST_CASE("Expr Parse"){
    SECTION("Parse Num"){
        CHECK(parse("3234") -> equals (new NumExpr(3234)));
        CHECK(parse(" 211") -> equals (new NumExpr(211)));
        CHECK(parse("-19 ") -> equals (new NumExpr(-19)));
        CHECK(parse("( -3    )") -> equals (new NumExpr(-3)));
        CHECK_THROWS_WITH(parse("(99"), "Missing closing paranthesis.");
    }

    SECTION("Parse Add"){
        CHECK(parse("2 + 1") -> equals (new AddExpr(new NumExpr(2), new NumExpr(1))));
        CHECK(parse("-7 + 6") -> equals (new AddExpr(new NumExpr(-7), new NumExpr(6))));
        CHECK(parse("(3 + 2)") -> equals (new AddExpr(new NumExpr(3), new NumExpr(2))));
        CHECK(parse("   5+1") -> equals (new AddExpr(new NumExpr(5), new NumExpr(1))));
        CHECK_THROWS_WITH(parse("(9 +"), "Invalid input (parse_inner).");
        CHECK_THROWS_WITH(parse("(9 +1"), "Missing closing paranthesis.");
        CHECK_THROWS_WITH(parse("9 +)"), "Invalid input (parse_inner).");
    }

    SECTION("Parse Mult"){
        CHECK(parse("6 * 12") -> equals (new MultExpr(new NumExpr(6), new NumExpr(12))));
        CHECK(parse("-1*2") -> equals (new MultExpr(new NumExpr(-1), new NumExpr(2))));
        CHECK(parse("(-8)*  4") -> equals (new MultExpr(new NumExpr(-8), new NumExpr(4))));
        CHECK(parse("(2  * 1)") -> equals (new MultExpr(new NumExpr(2), new NumExpr(1))));
        CHECK_THROWS_WITH(parse("(2  * 1"), "Missing closing paranthesis.");
        CHECK_THROWS_WITH(parse("2  * 1)"), "Invalid input (parse).");
        CHECK_THROWS_WITH(parse("(((((2  * 1)"), "Missing closing paranthesis.");
        CHECK_THROWS_WITH(parse("(2  * 1))))))"), "Invalid input (parse).");
    }

    SECTION("Parse Var"){
        CHECK(parse("cat") -> equals (new VarExpr("cat")));
        CHECK(parse("  dog") -> equals (new VarExpr("dog")));
        CHECK(parse("OWLS") -> equals (new VarExpr("OWLS")));
        CHECK(parse("wild_animals") -> equals (new VarExpr("wild_animals")));
        CHECK_THROWS_WITH(parse("wha.le"), "Invalid input (parse_var).");
    }

    SECTION("Parse Bool"){
        CHECK(parse("_true") -> equals(new BoolExpr(true)));
        CHECK(parse("_false") -> equals(new BoolExpr(false)));
        CHECK_THROWS_WITH(parse("_t"), "Invalid input (parse_inner, keyword error)");
        CHECK_THROWS_WITH(parse("_fals 1"), "Invalid input (parse_inner, keyword error)");
    }

    SECTION("Parse _let"){
        CHECK(parse("_let x = 5 _in x+2") -> equals
              (new LetExpr(new VarExpr("x"), new NumExpr(5),
                        new AddExpr(new VarExpr("x"), new NumExpr(2)))));
        CHECK(parse("_let x = (x+2) _in      (x+-3)") -> equals
              (new LetExpr(new VarExpr("x"), new AddExpr(new VarExpr("x"), new NumExpr(2)),
                        new AddExpr(new VarExpr("x"), new NumExpr(-3)))));
        CHECK_THROWS_WITH(parse("_let x = 1    _i"), "Invalid input (parse_let).");
    }


    SECTION("Parse _if"){
        CHECK(parse("_if _true _then 1 _else 0") -> equals(new IfExpr(new BoolExpr(true), new NumExpr(1), new NumExpr(0))));
        CHECK(parse("_if 3==2+1 _then a _else b") -> equals(new IfExpr(
                                                                new EqualExpr(new NumExpr(3),
                                                                              new AddExpr(new NumExpr(2), new NumExpr(1))),
                                                                new VarExpr("a"),
                                                                new VarExpr("b"))));
        CHECK_THROWS_WITH(parse("_if x = 1 then 2 else 1"), "Invalid input ('==' keyword).");
    }

    
    SECTION("Parse _fun"){
        CHECK(parse("_fun (x) x+2") -> equals(new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(2)))));
        CHECK(parse("_fun (test) y*1") -> equals(new FunExpr("test",
                                                             new MultExpr(new VarExpr("y"), new NumExpr(1)))));
        CHECK(parse("_fun (myFun) _let a = 1 _in a+2") -> equals(new FunExpr("myFun",
                                                                             new LetExpr(new VarExpr("a"),
                                                                                         new NumExpr(1),
                                                                                         new AddExpr(new VarExpr("a"),
                                                                                                     new NumExpr(2))))));
    }

}


TEST_CASE("NumVal Tests"){
    
    SECTION("NumVal equals()"){
        PTR(Val) test = new NumVal(3);
        CHECK(test -> equals (new NumVal(3)) == true);
        CHECK(test -> equals (new NumVal(5)) == false);
        CHECK(test -> equals ((new AddExpr(new NumExpr(1), new NumExpr(2))) -> interp()) == true);
        CHECK(test -> equals ((new MultExpr(new NumExpr(1), new NumExpr(2))) -> interp()) == false);
    }
    
    SECTION("NumVal toExpr()"){
        PTR(Val) test = new NumVal(-7);
        CHECK(((test -> to_expr()) -> equals(new NumExpr(-7))) == true);
        CHECK(((test -> to_expr()) -> equals(new NumExpr(7))) == false);
    }
    
    SECTION("NumVal to_string()"){
        PTR(Val) test = new NumVal(1010);
        CHECK(((test -> to_string()) == "1010") == true);
        CHECK(((test -> to_string()) == "0101") == false);
    }
    
    SECTION("NumVal add_to()"){
        PTR(Val) test1 = new NumVal(-5);
        PTR(Val) test2 = new NumVal(5);
        CHECK(((test1 -> add_to(test2)) -> equals (new NumVal(0))) == true);
        CHECK(((test1 -> add_to(test2)) -> equals (new NumVal(5))) == false);
        CHECK_THROWS_WITH(((test1 -> add_to(new BoolVal(true)))), "Error: rhs of Val add_to() must be a NumVal.");
    }
    
    SECTION("NumVal mult_to()"){
        PTR(Val) test1 = new NumVal(-5);
        PTR(Val) test2 = new NumVal(5);
        CHECK(((test1 -> mult_by(test2)) -> equals (new NumVal(-25))) == true);
        CHECK(((test1 -> mult_by(test2)) -> equals (new NumVal(25))) == false);
        CHECK_THROWS_WITH(((test1 -> mult_by(new BoolVal(false)))), "Error: rhs of Val mult_by() must be a NumVal.");
    }
}

TEST_CASE("BoolVal Tests"){

    SECTION("BoolVal equals()"){
        PTR(Val) test = new BoolVal(true);
        CHECK(test -> equals (new BoolVal(true)) == true);
        CHECK(test -> equals (new BoolVal(false)) == false);
        CHECK(test -> equals (new NumVal(1)) == false);
    }
    
    SECTION("BoolVal toExpr()"){
        PTR(Val) test = new BoolVal(true);
        CHECK(((test -> to_expr()) == NULL) == true);
    }
    
    SECTION("BoolVal to_string()"){
        PTR(Val) test1 = new BoolVal(true);
        CHECK(((test1 -> to_string()) == "_true") == true);
        CHECK(((test1 -> to_string()) == "_false") == false);
        PTR(Val) test2 = new BoolVal(false);
        CHECK(((test2 -> to_string()) == "_false") == true);
        CHECK(((test2 -> to_string()) == "_true") == false);
    }
    
    SECTION("BoolVal add_to()"){
        PTR(Val) test = new BoolVal(true);
        CHECK_THROWS_WITH((test -> add_to(new NumVal(1))), "Error: BoolVal cannot be added.");
        CHECK_THROWS_WITH((test -> add_to(new BoolVal(true))), "Error: BoolVal cannot be added.");
    }
    
    SECTION("BoolVal mult_to()"){
        PTR(Val) test = new BoolVal(true);
        CHECK_THROWS_WITH((test -> mult_by(new NumVal(1))), "Error: BoolVal cannot be multiplied.");
        CHECK_THROWS_WITH((test -> mult_by(new BoolVal(true))), "Error: BoolVal cannot be multiplied.");
    }
}

TEST_CASE("FunVal Tests"){
    PTR(Val) test = new FunVal("test", new AddExpr(new NumExpr(1), new VarExpr("x")));
    
    SECTION("FunVal equals()"){
        CHECK(test -> equals (new FunVal("test", new AddExpr(new NumExpr(1), new VarExpr("x")))) == true);
        CHECK(test -> equals (new FunVal("test", new AddExpr(new NumExpr(-1), new VarExpr("x")))) == false);
        CHECK(test -> equals (new FunVal("x", new AddExpr(new NumExpr(1), new VarExpr("x")))) == false);
    }
    
    SECTION("FunVal toExpr()"){
        CHECK((test -> to_expr()) -> equals(new FunExpr("test", new AddExpr(new NumExpr(1), new VarExpr("x")))));
        CHECK((test -> to_expr()) -> equals(new FunExpr("test", new AddExpr(new NumExpr(-1), new VarExpr("x")))) == false);
    }
    
    SECTION("FunVal to_string()"){
        CHECK((test -> to_string()) == "_fun (test) (1+x)");
        CHECK((test -> to_string()) != "(_fun (test) (1+x))");
    }
    
    SECTION("FunVal add_to()"){
        CHECK_THROWS_WITH((test -> add_to(new NumVal(1))), "Error: FunVal cannot be added.");
        CHECK_THROWS_WITH((test -> add_to(new BoolVal(true))), "Error: FunVal cannot be added.");
    }
    
    SECTION("FunVal mult_to()"){
        CHECK_THROWS_WITH((test -> mult_by(new NumVal(1))), "Error: FunVal cannot be multiplied.");
        CHECK_THROWS_WITH((test -> mult_by(new BoolVal(true))), "Error: FunVal cannot be multiplied.");
    }
}
