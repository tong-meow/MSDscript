//
//  tests.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/19/22.
//

#include <iostream>
#include <sstream>
#include "tests.hpp"

#include "expr.hpp"
#include "parse.hpp"
#include "val.hpp"
#include "env.hpp"
#include "pointer.hpp"
#include "step.hpp"
#include "cont.hpp"

// EQUAL TESTS

TEST_CASE("Expr Equals"){

    SECTION("Num Equals"){
        CHECK((NEW(NumExpr)(2))-> equals(NEW(NumExpr)(2)) == true);
        CHECK((NEW(NumExpr)(2))-> equals(NEW(NumExpr)(5)) == false);
        CHECK((NEW(NumExpr)(2))-> equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(5))) == false);
    }

    SECTION("Add Equals"){
        PTR(Expr) test = NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(5));
        CHECK(test-> equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(5))) == true);
        CHECK(test-> equals(NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(2))) == false);
        CHECK(test-> equals(NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(10))) == false);
        CHECK(test-> equals(NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(6))) == false);
    }

    SECTION("Mult Equals"){
        PTR(Expr) test = NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(6));
        CHECK(test-> equals(NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(6))) == true);
        CHECK(test-> equals(NEW(MultExpr)(NEW(NumExpr)(6), NEW(NumExpr)(3))) == false);
        CHECK(test-> equals(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2))) == false);
        CHECK(test-> equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(5))) == false);
    }

    SECTION("Var Equals"){
        PTR(Expr) test = NEW(VarExpr)("Happy2022");
        CHECK(test-> equals(NEW(VarExpr)("Happy2022")) == true);
        CHECK(test-> equals(NEW(VarExpr)("PlayStation")) == false);
        CHECK(test-> equals(NEW(VarExpr)("XBox")) == false);
        CHECK(test-> equals(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2))) == false);
    }

    SECTION("Let Equals"){
        // _let x = 5;
        // _in x + 1;
        PTR(LetExpr) test = NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)));
        CHECK(test-> equals(NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)))) == true);
        CHECK(test-> equals(NEW(LetExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(1)))) == false);
        CHECK(test-> equals(NEW(NumExpr)(6)) == false);
    }

    SECTION("Bool Equals"){
        PTR(Expr) test = NEW(BoolExpr)(true);
        CHECK((test -> equals(NEW(BoolExpr)(true))) == true);
        CHECK((test -> equals(NEW(BoolExpr)(false))) == false);
        CHECK((test -> equals(NEW(NumExpr)(3))) == false);
    }

    SECTION("Equal Equals"){
        PTR(Expr) test = NEW(EqualExpr)(NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)));
        CHECK((test -> equals(NEW(EqualExpr)(NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2))))) == true);
        CHECK((test -> equals(NEW(EqualExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) == false);
        CHECK((test -> equals(NEW(NumExpr)(3))) == false);
    }

    SECTION("If Equals"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test = NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(0), NEW(AddExpr)(NEW(NumExpr)(-1), NEW(NumExpr)(1))),
                                NEW(NumExpr)(1), NEW(NumExpr)(0));
        CHECK((test -> equals(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(0), NEW(AddExpr)(NEW(NumExpr)(-1), NEW(NumExpr)(1))),
                                         NEW(NumExpr)(1), NEW(NumExpr)(0)))) == true);
        CHECK((test -> equals(NEW(NumExpr)(3))) == false);
    }

    SECTION("Fun Equals"){
        // _fun (x) x + 2
        PTR(Expr) test = NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)));
        CHECK(test -> equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)))));
        CHECK(test -> equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))) == false);
    }

    SECTION("Call Equals"){
        PTR(Expr) test = NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2))), NEW(NumExpr)(2));
        CHECK(test -> equals(NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2))),
                                          NEW(NumExpr)(2))));
        CHECK(test -> equals(NEW(CallExpr)(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2))),
                                          NEW(NumExpr)(-2)))
              == false);
    }

}


// INTERP TESTS

TEST_CASE("Expr Interp"){
    SECTION("Num Interp"){
        PTR(Expr) test = NEW(NumExpr)(42);
        CHECK(test-> interp(NEW(EmptyEnv)()) -> equals(NEW(NumVal)(42)));
    }

    SECTION("Add Interp"){
        // test == 1 + 2 + 3 == 6
        PTR(Expr) test = NEW(AddExpr)(NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)), NEW(NumExpr)(3));
        // test == 6
        CHECK(test-> interp(NEW(EmptyEnv)()) -> equals (NEW(NumVal)(6)));
        // throws exception
        CHECK_THROWS_WITH((NEW(AddExpr)(NEW(VarExpr)("Wrong"),
                                        NEW(NumExpr)(1)))
                          -> interp(NEW(EmptyEnv)()), "Free variable: Wrong" );
    }

    SECTION("Mult Interp"){
        // test == 1 * 2 * 3 == 6
        PTR(Expr) test = NEW(MultExpr)(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)), NEW(NumExpr)(3));
        // test == 6
        CHECK(test->interp(NEW(EmptyEnv)()) -> equals (NEW(NumVal)(6)));
        // throws exception
        CHECK_THROWS_WITH((NEW(MultExpr)(NEW(VarExpr)("Wrong"),
                                         NEW(NumExpr)(1)))
                          -> interp(NEW(EmptyEnv)()), "Free variable: Wrong" );
    }

    SECTION("Var Interp"){
        // test = "owls"
        PTR(Expr) test = NEW(VarExpr)("owls");
        // throws exception
        CHECK_THROWS_WITH(test -> interp(NEW(EmptyEnv)()), "Free variable: owls");
    }

    SECTION("Let Interp"){
        // test1 =
        //        _let x = 5;
        //        _in 6;
        PTR(Expr) test1 = NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5), NEW(NumExpr)(6));
        CHECK(test1 -> interp(NEW(EmptyEnv)()) -> equals (NEW(NumVal)(6)));
        // test2 =
        //        _let x = 5;
        //        _in x + 1;
        PTR(Expr) test2 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(NumExpr)(5),
                               NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)));
        CHECK(test2 -> interp(NEW(EmptyEnv)()) -> equals (NEW(NumVal)(6)));
        // test3 =
        //        _let x = 5;
        //        _in y + 1;
        PTR(Expr) test3 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(NumExpr)(5),
                               NEW(AddExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(1)));
        CHECK_THROWS_WITH( test3 -> interp(NEW(EmptyEnv)()), "Free variable: y" );
        // test4 =
        //        _let x = 5;
        //        _in _let x = 3;
        //            _in x + 1;
        PTR(Expr) test4 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(NumExpr)(5),
                               NEW(LetExpr)(NEW(VarExpr)("x"),
                                        NEW(NumExpr)(3),
                                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))));
        CHECK(test4 -> interp(NEW(EmptyEnv)()) -> equals (NEW(NumVal)(4)));
        // test5 =
        //        _let x = 5;
        //        _in _let y = 3;
        //            _in x + 1;
        PTR(Expr) test5 = NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5),
                               NEW(LetExpr)(NEW(VarExpr)("y"),
                                        NEW(NumExpr)(3),
                                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))));
        CHECK(test5 -> interp(NEW(EmptyEnv)()) -> equals (NEW(NumVal)(6)));
        // test6 =
        //        _let x = 5;
        //        _in _let x = x + 1;
        //            _in x + 1;
        PTR(Expr) test6 = NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5),
                               NEW(LetExpr)(NEW(VarExpr)("x"),
                                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)),
                                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))));
        CHECK(test6 -> interp(NEW(EmptyEnv)()) -> equals (NEW(NumVal)(7)));
        // test7 =
        //        _let x = x + 1;
        //        _in x + 1;
        PTR(Expr) test7 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)),
                               NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)));
        CHECK_THROWS_WITH( test7 -> interp(NEW(EmptyEnv)()),
                          "Free variable: x" );
        // test8 =
        //        _let x = _let x = 3
        //                 _in x + 1
        //        _in x + 1
        PTR(Expr) test8 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3),
                                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))),
                               NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)));
        CHECK(test8 -> interp(NEW(EmptyEnv)()) -> equals (NEW(NumVal)(5)));
    }

    SECTION("BoolExpr Interp"){
        PTR(Expr) test = NEW(BoolExpr)(false);
        CHECK(test -> interp(NEW(EmptyEnv)()) -> equals(NEW(BoolVal)(false)));
        CHECK(test -> interp(NEW(EmptyEnv)()) -> equals(NEW(BoolVal)(true)) == false);
    }

    SECTION("EqualExpr Interp"){
        PTR(Expr) test1 = NEW(EqualExpr)(NEW(NumExpr)(3),
                                         NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)));
        CHECK(test1 -> interp(NEW(EmptyEnv)()) -> equals(NEW(BoolVal)(true)) == true);
        CHECK(test1 -> interp(NEW(EmptyEnv)()) -> equals(NEW(BoolVal)(false)) == false);
        PTR(Expr) test2 = NEW(EqualExpr)(NEW(NumExpr)(3),
                                         NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(-2)));
        CHECK(test2 -> interp(NEW(EmptyEnv)()) -> equals(NEW(BoolVal)(false)) == true);
        CHECK(test2 -> interp(NEW(EmptyEnv)()) -> equals(NEW(BoolVal)(true)) == false);
    }

    SECTION("IfExpr Interp"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test1 = NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(0),
                                                     NEW(AddExpr)(NEW(NumExpr)(-1),
                                                                  NEW(NumExpr)(1))),
                                      NEW(NumExpr)(1),
                                      NEW(NumExpr)(0));
        CHECK(test1 -> interp(NEW(EmptyEnv)()) -> equals(NEW(NumVal)(1)) == true);
        CHECK(test1 -> interp(NEW(EmptyEnv)()) -> equals(NEW(NumVal)(0)) == false);
        // _if 1 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test2 = NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(1),
                                                     NEW(AddExpr)(NEW(NumExpr)(-1),
                                                                  NEW(NumExpr)(1))),
                                      NEW(NumExpr)(1),
                                      NEW(NumExpr)(0));
        CHECK(test2 -> interp(NEW(EmptyEnv)()) -> equals(NEW(NumVal)(1)) == false);
        CHECK(test2 -> interp(NEW(EmptyEnv)()) -> equals(NEW(NumVal)(0)) == true);
        // if 4
        // _then 1
        // _else 0
        PTR(Expr) test3 = NEW(IfExpr)(NEW(NumExpr)(4),NEW(NumExpr)(1), NEW(NumExpr)(0));
        CHECK_THROWS_WITH(test3 -> interp(NEW(EmptyEnv)()),
                          "IfExpr's condition is not a boolean value.");
    }

    SECTION("FunExpr Interp"){
        PTR(Expr) test = NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)));
        CHECK(test -> interp(NEW(EmptyEnv)())
              -> equals(NEW(FunVal)("x",
                                    NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)),
                                    NEW(EmptyEnv)())));
        CHECK(test -> interp(NEW(EmptyEnv)())
              -> equals(NEW(FunVal)("y",
                                    NEW(AddExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(2)),
                                    NEW(EmptyEnv)())) == false);
    }

    SECTION("CallExpr Interp"){
        PTR(Expr) fun = NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)));
        PTR(Expr) test = NEW(CallExpr)(fun, NEW(NumExpr)(2));
        CHECK(test -> interp(NEW(EmptyEnv)())
              -> equals(fun -> interp(NEW(EmptyEnv)())
                        -> call((NEW(NumExpr)(2)) -> interp(NEW(EmptyEnv)()))));
    }
}


// PRINT TESTS

TEST_CASE("Expr Print"){
    SECTION("Num Print"){
        PTR(Expr) test1 = NEW(NumExpr)(10);
        CHECK((test1 -> to_string()) == "10");
    }

    SECTION("Add Print"){
        PTR(Expr) test1 = NEW(AddExpr)(NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)));
        CHECK((test1 -> to_string()) == "(3+(1+2))");
        PTR(Expr) test2 = NEW(AddExpr)(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(1)), NEW(NumExpr)(2));
        CHECK((test2 -> to_string()) == "((3+1)+2)");
    }

    SECTION("Mult Print"){
        PTR(Expr) test1 = NEW(MultExpr)(NEW(NumExpr)(3), NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)));
        CHECK((test1 -> to_string()) == "(3*(1*2))");
        PTR(Expr) test2 = NEW(MultExpr)(NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(1)), NEW(NumExpr)(2));
        CHECK((test2 -> to_string()) == "((3*1)*2)");
    }

    SECTION("Variable Print"){
        PTR(Expr) test1 = NEW(VarExpr)("X");
        CHECK((test1 -> to_string()) == "X");
    }

    SECTION("Let Print"){
        // test1 =
        //        _let x = 5;
        //        _in x + 1;
        PTR(Expr) test1 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(NumExpr)(5),
                               NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)));
        CHECK((test1 -> to_string()) == "(_let x=5 _in (x+1))");
        // test2 =
        //        _let x = 5;
        //        _in _let x = 3;
        //            _in x + 1;
        PTR(Expr) test2 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(NumExpr)(5),
                               NEW(LetExpr)(NEW(VarExpr)("x"),
                                        NEW(NumExpr)(3),
                                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))));
        CHECK((test2 -> to_string()) == "(_let x=5 _in (_let x=3 _in (x+1)))");
        // test3 =
        //        _let x = x + 1;
        //        _in x + 1;
        PTR(Expr) test3 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)),
                               NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)));
        CHECK((test3 -> to_string()) == "(_let x=(x+1) _in (x+1))");
    }

    SECTION("boolExpr Print"){
        PTR(Expr) test1 = NEW(BoolExpr)(true);
        CHECK((test1 -> to_string()) == "_true");
        PTR(Expr) test2 = NEW(BoolExpr)(false);
        CHECK((test2 -> to_string()) == "_false");
    }

    SECTION("EqualExpr Print"){
        PTR(Expr) test1 = NEW(EqualExpr)(NEW(VarExpr)("grade"), NEW(NumExpr)(99));
        CHECK((test1 -> to_string()) == "(grade == 99)");
        PTR(Expr) test2 = NEW(EqualExpr)(NEW(VarExpr)("grade"), NEW(AddExpr)(NEW(NumExpr)(30), NEW(NumExpr)(60)));
        CHECK((test2 -> to_string()) == "(grade == (30+60))");
    }

    SECTION("IfExpr Print"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test1 = NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(0), NEW(AddExpr)(NEW(NumExpr)(-1), NEW(NumExpr)(1))),
                                NEW(NumExpr)(1), NEW(NumExpr)(0));
        CHECK((test1 -> to_string()) == "(_if ((0 == (-1+1))) _then (1) _else (0))");
        // if good
        // _then 1
        // _else 0
        PTR(Expr) test2 = NEW(IfExpr)(NEW(VarExpr)("good"),NEW(NumExpr)(1), NEW(NumExpr)(0));
        CHECK((test2 -> to_string()) == "(_if (good) _then (1) _else (0))");
    }

    SECTION("FunExpr Print"){
        PTR(Expr) test = NEW(FunExpr)("testFun", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)));
        CHECK(test -> to_string() == "(_fun (testFun) (x+2))");
    }

    SECTION("CallExpr Print"){
        PTR(Expr) fun = NEW(FunExpr)("testFun", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)));
        PTR(Expr) test = NEW(CallExpr)(fun, NEW(NumExpr)(2));
        CHECK(test -> to_string() == "(_fun (testFun) (x+2))(2)");
    }
}

// PRETTY_PRINT TESTS

TEST_CASE("Expr Pretty Print"){
    SECTION("Num Pretty Print"){
        PTR(Expr) test1 = NEW(NumExpr)(10);
        CHECK((test1 -> to_pretty_string()) == "10");
    }

    SECTION("Add Pretty Print"){
        PTR(Expr) test1 = NEW(AddExpr)(NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)));
        CHECK((test1 -> to_pretty_string()) == "3 + 1 + 2");
        PTR(Expr) test2 = NEW(AddExpr)(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(1)), NEW(NumExpr)(2));
        CHECK((test2 -> to_pretty_string()) == "(3 + 1) + 2");
    }

    SECTION("Mult Pretty Print"){
        PTR(Expr) test1 = NEW(MultExpr)(NEW(NumExpr)(3), NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)));
        CHECK((test1 -> to_pretty_string()) == "3 * 1 * 2");
        PTR(Expr) test2 = NEW(MultExpr)(NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(1)), NEW(NumExpr)(2));
        CHECK((test2 -> to_pretty_string()) == "(3 * 1) * 2");
    }

    SECTION("Add and Mult Mixed Pretty Print"){
        // 1 + 2 * 3
        PTR(Expr) test1 = NEW(AddExpr)(NEW(NumExpr)(1), NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)));
        CHECK((test1 -> to_pretty_string()) == "1 + 2 * 3");
        // 1 * (2 + 3)
        PTR(Expr) test2 = NEW(MultExpr)(NEW(NumExpr)(1), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)));
        CHECK((test2 -> to_pretty_string()) == "1 * (2 + 3)");
        // 1 * (2 + 3) * 4
        PTR(Expr) test3 = NEW(MultExpr)(NEW(NumExpr)(1), NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)), NEW(NumExpr)(4)));
        CHECK((test3 -> to_pretty_string()) == "1 * (2 + 3) * 4");
        // (1 + 2) * (3 + 4)
        PTR(Expr) test4 = NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)), NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)));
        CHECK((test4 -> to_pretty_string()) == "(1 + 2) * (3 + 4)");
        // (1 * (2 + 3)) * 4
        PTR(Expr) test5 = NEW(MultExpr)(NEW(MultExpr)(NEW(NumExpr)(1), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))), NEW(NumExpr)(4));
        CHECK((test5 -> to_pretty_string()) == "(1 * (2 + 3)) * 4");
    }

    SECTION("Var Pretty Print"){
        PTR(Expr) test1 = NEW(VarExpr)("Y");
        CHECK((test1 -> to_pretty_string()) == "Y");
    }

    SECTION("_let Pretty Print"){
        // test1 =
        //        _let x = 5
        //        _in x + 1
        PTR(Expr) test1 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(NumExpr)(5),
                               NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)));
        CHECK((test1 -> to_pretty_string()) == "_let x = 5\n_in x + 1");
        // test2 =
        //        (_let x = 5
        //        _in x) + 1
        /*//////BUG NEED TO BE FIXED//////*/
//        PTR(Expr) test2 = NEW(AddExpr)(NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5), NEW(VarExpr)("x")),
//                              NEW(NumExpr)(1));
//        CHECK((test2 -> to_pretty_string()) == "(_let x = 5\n _in x) + 1");
        // test3 =
        //        5 * (_let x = 5
        //             _in  x) + 1
//        PTR(Expr) test3 = NEW(AddExpr) (NEW(MultExpr)(NEW(NumExpr)(5),
//                                        NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5), NEW(VarExpr)("x"))),
//                               NEW(NumExpr)(1));
//        CHECK((test3 -> to_pretty_string()) == "5 * (_let x = 5\n"
//                                               "     _in x) + 1");
        // test4 =
        //        5 * _let x = 5
        //            _in  x + 1
        PTR(Expr) test4 = NEW(MultExpr)(NEW(NumExpr)(5),
                               NEW(LetExpr)(NEW(VarExpr)("x"),
                                        NEW(NumExpr)(5),
                                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))));
        CHECK((test4 -> to_pretty_string()) == "5 * _let x = 5\n"
                                               "    _in x + 1");
        // test5 =
        //        _let x = 5
        //        _in _let x = 3
        //            _in x + 1
        PTR(Expr) test5 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(NumExpr)(5),
                               NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3),
                                       NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))));
        CHECK((test5 -> to_pretty_string()) == "_let x = 5\n"
                                               "_in _let x = 3\n"
                                               "    _in x + 1");
        // test6 =
        //        _let x = _let x = 3
        //                 _in x + 1
        //        _in x + 1
        PTR(Expr) test6 = NEW(LetExpr)(NEW(VarExpr)("x"),
                               NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3),
                                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))),
                               NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)));
        CHECK((test6 -> to_pretty_string()) == "_let x = _let x = 3\n"
                                               "         _in x + 1\n"
                                               "_in x + 1");
        // test7 =
        //       (5 + (_let x = _let x = 1
        //                      _in x + 2
        //             _in x + 3)) * 4
        /*//////BUG NEED TO BE FIXED//////*/
//        PTR(Expr) test7 = NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(5),
//                                       NEW(LetExpr)(NEW(VarExpr)("x"),
//                                                NEW(LetExpr)(NEW(VarExpr)("x"),
//                                                         NEW(NumExpr)(1),
//                                                         NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2))),
//                                                NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))),
//                               NEW(NumExpr)(4));
//        CHECK((test7 -> to_pretty_string()) == "(5 + (_let x = _let x = 1\n"
//                                               "               _in x + 2\n"
//                                               "      _in x + 3)) * 4");
    }

    SECTION("boolExpr Pretty Print"){
        PTR(Expr) test1 = NEW(BoolExpr)(true);
        CHECK((test1 -> to_pretty_string()) == "_true");
        PTR(Expr) test2 = NEW(BoolExpr)(false);
        CHECK((test2 -> to_pretty_string()) == "_false");
    }

    SECTION("EqualExpr Pretty Print"){
        PTR(Expr) test1 = NEW(EqualExpr)(NEW(VarExpr)("grade"), NEW(NumExpr)(99));
        CHECK((test1 -> to_pretty_string()) == "grade == 99");
        PTR(Expr) test2 = NEW(EqualExpr)(NEW(VarExpr)("grade"), NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(80)));
        CHECK((test2 -> to_pretty_string()) == "grade == 10 + 80");
    }

    SECTION("IfExpr Pretty Print"){
        // _if 0 == -1 + 1
        // _then 1
        // _else 0
        PTR(Expr) test1 = NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(0), NEW(AddExpr)(NEW(NumExpr)(-1), NEW(NumExpr)(1))),
                                NEW(NumExpr)(1), NEW(NumExpr)(0));
        CHECK((test1 -> to_pretty_string()) == "_if 0 == -1 + 1\n"
                                               "_then 1\n"
                                               "_else 0");
        // if good
        // _then 1
        // _else 0
        PTR(Expr) test2 = NEW(IfExpr)(NEW(VarExpr)("good"),NEW(NumExpr)(1), NEW(NumExpr)(0));
        CHECK((test2 -> to_pretty_string()) == "_if good\n"
                                               "_then 1\n"
                                               "_else 0");
    }
}



// PARSE TESTS

TEST_CASE("Expr Parse"){
    SECTION("Parse Num"){
        CHECK(parse("3234") -> equals (NEW(NumExpr)(3234)));
        CHECK(parse(" 211") -> equals (NEW(NumExpr)(211)));
        CHECK(parse("-19 ") -> equals (NEW(NumExpr)(-19)));
        CHECK(parse("( -3    )") -> equals (NEW(NumExpr)(-3)));
        CHECK_THROWS_WITH(parse("(99"), "Missing closing paranthesis.");
    }

    SECTION("Parse Add"){
        CHECK(parse("2 + 1") -> equals (NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(1))));
        CHECK(parse("-7 + 6") -> equals (NEW(AddExpr)(NEW(NumExpr)(-7), NEW(NumExpr)(6))));
        CHECK(parse("(3 + 2)") -> equals (NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2))));
        CHECK(parse("   5+1") -> equals (NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(1))));
        CHECK_THROWS_WITH(parse("(9 +"), "Invalid input (parse_inner).");
        CHECK_THROWS_WITH(parse("(9 +1"), "Missing closing paranthesis.");
        CHECK_THROWS_WITH(parse("9 +)"), "Invalid input (parse_inner).");
    }

    SECTION("Parse Mult"){
        CHECK(parse("6 * 12") -> equals (NEW(MultExpr)(NEW(NumExpr)(6), NEW(NumExpr)(12))));
        CHECK(parse("-1*2") -> equals (NEW(MultExpr)(NEW(NumExpr)(-1), NEW(NumExpr)(2))));
        CHECK(parse("(-8)*  4") -> equals (NEW(MultExpr)(NEW(NumExpr)(-8), NEW(NumExpr)(4))));
        CHECK(parse("(2  * 1)") -> equals (NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(1))));
        CHECK_THROWS_WITH(parse("(2  * 1"), "Missing closing paranthesis.");
        CHECK_THROWS_WITH(parse("2  * 1)"), "Invalid input (parse).");
        CHECK_THROWS_WITH(parse("(((((2  * 1)"), "Missing closing paranthesis.");
        CHECK_THROWS_WITH(parse("(2  * 1))))))"), "Invalid input (parse).");
    }

    SECTION("Parse Var"){
        CHECK(parse("cat") -> equals (NEW(VarExpr)("cat")));
        CHECK(parse("  dog") -> equals (NEW(VarExpr)("dog")));
        CHECK(parse("OWLS") -> equals (NEW(VarExpr)("OWLS")));
        CHECK(parse("wild_animals") -> equals (NEW(VarExpr)("wild_animals")));
        CHECK_THROWS_WITH(parse("wha.le"), "Invalid input (parse_var).");
    }

    SECTION("Parse Bool"){
        CHECK(parse("_true") -> equals(NEW(BoolExpr)(true)));
        CHECK(parse("_false") -> equals(NEW(BoolExpr)(false)));
        CHECK_THROWS_WITH(parse("_t"), "Invalid input (parse_inner, keyword error)");
        CHECK_THROWS_WITH(parse("_fals 1"), "Invalid input (parse_inner, keyword error)");
    }

    SECTION("Parse _let"){
        CHECK(parse("_let x = 5 _in x+2") -> equals
              (NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5),
                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)))));
        CHECK(parse("_let x = (x+2) _in      (x+-3)") -> equals
              (NEW(LetExpr)(NEW(VarExpr)("x"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)),
                        NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(-3)))));
        CHECK_THROWS_WITH(parse("_let x = 1    _i"), "Invalid input (parse_let).");
    }


    SECTION("Parse _if"){
        CHECK(parse("_if _true _then 1 _else 0") -> equals(NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(1), NEW(NumExpr)(0))));
        CHECK(parse("_if 3==2+1 _then a _else b") -> equals(NEW(IfExpr)(
                                                                NEW(EqualExpr)(NEW(NumExpr)(3),
                                                                              NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(1))),
                                                                NEW(VarExpr)("a"),
                                                                NEW(VarExpr)("b"))));
        CHECK_THROWS_WITH(parse("_if x = 1 then 2 else 1"), "Invalid input ('==' keyword).");
    }


    SECTION("Parse _fun"){
        CHECK(parse("_fun (x) x+2") -> equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)))));
        CHECK(parse("_fun (test) y*1") -> equals(NEW(FunExpr)("test",
                                                             NEW(MultExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(1)))));
        CHECK(parse("_fun (myFun) _let a = 1 _in a+2") -> equals(NEW(FunExpr)("myFun",
                                                                             NEW(LetExpr)(NEW(VarExpr)("a"),
                                                                                         NEW(NumExpr)(1),
                                                                                         NEW(AddExpr)(NEW(VarExpr)("a"),
                                                                                                     NEW(NumExpr)(2))))));
    }

}


TEST_CASE("NumVal Tests"){

    SECTION("NumVal equals()"){
        PTR(Val) test = NEW(NumVal)(3);
        CHECK(test -> equals (NEW(NumVal)(3)) == true);
        CHECK(test -> equals (NEW(NumVal)(5)) == false);
        CHECK(test -> equals ((NEW(AddExpr)(NEW(NumExpr)(1),
                                           NEW(NumExpr)(2)))
                              -> interp(NEW(EmptyEnv)())) == true);
        CHECK(test -> equals ((NEW(MultExpr)(NEW(NumExpr)(1),
                                            NEW(NumExpr)(2)))
                              -> interp(NEW(EmptyEnv)())) == false);
    }

    SECTION("NumVal toExpr()"){
        PTR(Val) test = NEW(NumVal)(-7);
        CHECK((test -> to_expr()) -> equals(NEW(NumExpr)(-7)) == true);
        CHECK((test -> to_expr()) -> equals(NEW(NumExpr)(7)) == false);
    }

    SECTION("NumVal to_string()"){
        PTR(Val) test = NEW(NumVal)(1010);
        CHECK(((test -> to_string()) == "1010") == true);
        CHECK(((test -> to_string()) == "0101") == false);
    }

    SECTION("NumVal add_to()"){
        PTR(Val) test1 = NEW(NumVal)(-5);
        PTR(Val) test2 = NEW(NumVal)(5);
        CHECK(((test1 -> add_to(test2)) -> equals (NEW(NumVal)(0))) == true);
        CHECK(((test1 -> add_to(test2)) -> equals (NEW(NumVal)(5))) == false);
        CHECK_THROWS_WITH(((test1 -> add_to(NEW(BoolVal)(true)))), "Error: rhs of Val add_to() must be a NumVal.");
    }

    SECTION("NumVal mult_to()"){
        PTR(Val) test1 = NEW(NumVal)(-5);
        PTR(Val) test2 = NEW(NumVal)(5);
        CHECK(((test1 -> mult_by(test2)) -> equals (NEW(NumVal)(-25))) == true);
        CHECK(((test1 -> mult_by(test2)) -> equals (NEW(NumVal)(25))) == false);
        CHECK_THROWS_WITH(((test1 -> mult_by(NEW(BoolVal)(false)))), "Error: rhs of Val mult_by() must be a NumVal.");
    }
}

TEST_CASE("BoolVal Tests"){

    SECTION("BoolVal equals()"){
        PTR(Val) test = NEW(BoolVal)(true);
        CHECK(test -> equals (NEW(BoolVal)(true)) == true);
        CHECK(test -> equals (NEW(BoolVal)(false)) == false);
        CHECK(test -> equals (NEW(NumVal)(1)) == false);
    }

    SECTION("BoolVal toExpr()"){
        PTR(Val) test = NEW(BoolVal)(true);
        CHECK((test -> to_expr() == NULL) == true);
    }

    SECTION("BoolVal to_string()"){
        PTR(Val) test1 = NEW(BoolVal)(true);
        CHECK(((test1 -> to_string()) == "_true") == true);
        CHECK(((test1 -> to_string()) == "_false") == false);
        PTR(Val) test2 = NEW(BoolVal)(false);
        CHECK(((test2 -> to_string()) == "_false") == true);
        CHECK(((test2 -> to_string()) == "_true") == false);
    }

    SECTION("BoolVal add_to()"){
        PTR(Val) test = NEW(BoolVal)(true);
        CHECK_THROWS_WITH((test -> add_to(NEW(NumVal)(1))), "Error: BoolVal cannot be added.");
        CHECK_THROWS_WITH((test -> add_to(NEW(BoolVal)(true))), "Error: BoolVal cannot be added.");
    }

    SECTION("BoolVal mult_to()"){
        PTR(Val) test = NEW(BoolVal)(true);
        CHECK_THROWS_WITH((test -> mult_by(NEW(NumVal)(1))), "Error: BoolVal cannot be multiplied.");
        CHECK_THROWS_WITH((test -> mult_by(NEW(BoolVal)(true))), "Error: BoolVal cannot be multiplied.");
    }
}

TEST_CASE("FunVal Tests"){
    PTR(Val) test = NEW(FunVal)("test",
                                NEW(AddExpr)(NEW(NumExpr)(1),
                                             NEW(VarExpr)("x")),
                                NEW(EmptyEnv)());

    SECTION("FunVal equals()"){
        CHECK(test -> equals (NEW(FunVal)("test",
                                          NEW(AddExpr)(NEW(NumExpr)(1), NEW(VarExpr)("x")),
                                          NEW(EmptyEnv)())) == true);
        CHECK(test -> equals (NEW(FunVal)("test",
                                          NEW(AddExpr)(NEW(NumExpr)(-1), NEW(VarExpr)("x")),
                                          NEW(EmptyEnv)())) == false);
        CHECK(test -> equals (NEW(FunVal)("x",
                                          NEW(AddExpr)(NEW(NumExpr)(1), NEW(VarExpr)("x")),
                                          NEW(EmptyEnv)())) == false);
    }

    SECTION("FunVal toExpr()"){
        CHECK((test -> to_expr()) -> equals(NEW(FunExpr)("test", NEW(AddExpr)(NEW(NumExpr)(1), NEW(VarExpr)("x")))));
        CHECK((test -> to_expr()) -> equals(NEW(FunExpr)("test", NEW(AddExpr)(NEW(NumExpr)(-1), NEW(VarExpr)("x")))) == false);
    }

    SECTION("FunVal to_string()"){
        CHECK((test -> to_string()) == "_fun (test) (1+x)");
        CHECK((test -> to_string()) != "(_fun (test) (1+x))");
    }

    SECTION("FunVal add_to()"){
        CHECK_THROWS_WITH((test -> add_to(NEW(NumVal)(1))), "Error: FunVal cannot be added.");
        CHECK_THROWS_WITH((test -> add_to(NEW(BoolVal)(true))), "Error: FunVal cannot be added.");
    }

    SECTION("FunVal mult_to()"){
        CHECK_THROWS_WITH((test -> mult_by(NEW(NumVal)(1))), "Error: FunVal cannot be multiplied.");
        CHECK_THROWS_WITH((test -> mult_by(NEW(BoolVal)(true))), "Error: FunVal cannot be multiplied.");
    }
}


TEST_CASE("Step Tests") {
    
    SECTION("NumExpr") {
        CHECK(Step::interp_by_steps(NEW(NumExpr)(5)) ->equals (NEW(NumVal)(5)) == true);
        CHECK(Step::interp_by_steps(NEW(NumExpr)(-5))->equals(NEW(NumVal)(5)) == false);
    }
    
    SECTION("AddExpr"){
        CHECK(Step::interp_by_steps(NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)))
              -> equals (NEW(NumVal)(3)) == true);
        CHECK(Step::interp_by_steps(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))
              -> equals(NEW(NumVal)(6)) == false);
    }
    
    SECTION("MultExpr") {
        CHECK(Step::interp_by_steps(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)))
              -> equals(NEW(NumVal)(2)) == true);
        CHECK(Step::interp_by_steps(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)))
              -> equals(NEW(NumVal)(-2)) == false);
    }
    
    SECTION("LetExpr") {
        CHECK(Step::interp_by_steps(NEW(LetExpr) (NEW(VarExpr)("x"), NEW(NumExpr)(1), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))))
              -> equals(NEW(NumVal)(2)) == true);
        CHECK(Step::interp_by_steps(NEW(LetExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5))))
              -> equals(NEW(NumVal)(2)) == false);
    }
    
    SECTION("BoolExpr") {
        CHECK(Step::interp_by_steps(NEW(BoolExpr)(true))
              -> equals(NEW(BoolVal)(true)) == true);
        CHECK(Step::interp_by_steps(NEW(BoolExpr)(true))
              -> equals(NEW(BoolVal)(false)) == false);
    }
    
    SECTION("EqualExpr") {
        CHECK(Step::interp_by_steps(NEW(EqualExpr)(NEW(NumExpr)(2), NEW(NumExpr)(2)))
              -> equals(NEW(BoolVal)(true)) == true);
        CHECK(Step::interp_by_steps(NEW(EqualExpr)(NEW(NumExpr)(4), NEW(NumExpr)(3)))
              -> equals(NEW(BoolVal)(false)) == true);
    }
    
    SECTION("IfExpr") {
        CHECK(Step::interp_by_steps(NEW(IfExpr)(NEW(BoolExpr)(true),
                                               NEW(NumExpr)(1),
                                               NEW(NumExpr)(0)))
              -> equals(NEW(NumVal)(1)));
        CHECK(Step::interp_by_steps(NEW(IfExpr)(NEW(BoolExpr)(false),
                                               NEW(NumExpr)(1),
                                               NEW(NumExpr)(0)))
              -> equals(NEW(NumVal)(0)));
        CHECK(Step::interp_by_steps(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(1),
                                                             NEW(NumExpr)(1)),
                                               NEW(NumExpr)(1),
                                               NEW(NumExpr)(0)))
              -> equals(NEW(NumVal)(1)));
        CHECK(Step::interp_by_steps(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(1),
                                                             NEW(NumExpr)(-1)),
                                               NEW(NumExpr)(1),
                                               NEW(NumExpr)(0)))
              -> equals(NEW(NumVal)(0)));
    }
    
    SECTION("FunExpr") {
        CHECK(Step::interp_by_steps(NEW(FunExpr)("x",
                                                 NEW(NumExpr)(1)))
              -> equals(NEW(FunVal)("x",
                                    NEW(NumExpr)(1),
                                    NEW(EmptyEnv)())) == true);
        CHECK(Step::interp_by_steps(NEW(FunExpr)("x", NEW(AddExpr)(NEW(NumExpr)(1),
                                                                   NEW(VarExpr)("x"))))
              -> equals (NEW(FunVal)("x",
                                    NEW(AddExpr)(NEW(NumExpr)(1),
                                                 NEW(VarExpr)("x")),
                                    NEW(EmptyEnv)())) == true);
    }
}
