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

// EQUAL TESTS

TEST_CASE("Num Equals"){
    CHECK((new NumExpr(2))-> equals(new NumExpr(2)) == true);
    CHECK((new NumExpr(2))-> equals(new NumExpr(5)) == false);
    CHECK((new NumExpr(2))-> equals(new AddExpr(new NumExpr(2), new NumExpr(5))) == false);
}

TEST_CASE("Add Equals"){
    Expr *test = new AddExpr(new NumExpr(2), new NumExpr(5));
    CHECK(test-> equals(new AddExpr(new NumExpr(2), new NumExpr(5))) == true);
    CHECK(test-> equals(new AddExpr(new NumExpr(5), new NumExpr(2))) == false);
    CHECK(test-> equals(new AddExpr(new NumExpr(4), new NumExpr(10))) == false);
    CHECK(test-> equals(new MultExpr(new NumExpr(3), new NumExpr(6))) == false);
}

TEST_CASE("Mult Equals"){
    Expr *test = new MultExpr(new NumExpr(3), new NumExpr(6));
    CHECK(test-> equals(new MultExpr(new NumExpr(3), new NumExpr(6))) == true);
    CHECK(test-> equals(new MultExpr(new NumExpr(6), new NumExpr(3))) == false);
    CHECK(test-> equals(new MultExpr(new NumExpr(1), new NumExpr(2))) == false);
    CHECK(test-> equals(new AddExpr(new NumExpr(2), new NumExpr(5))) == false);
}

TEST_CASE("Var Equals"){
    Expr *test = new VarExpr("Happy2022");
    CHECK(test-> equals(new VarExpr("Happy2022")) == true);
    CHECK(test-> equals(new VarExpr("PlayStation")) == false);
    CHECK(test-> equals(new VarExpr("XBox")) == false);
    CHECK(test-> equals(new MultExpr(new NumExpr(1), new NumExpr(2))) == false);
}

TEST_CASE("Let Equals"){
    // _let x = 5;
    // _in x + 1;
    LetExpr *test = new LetExpr(new VarExpr("x"), new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK(test-> equals(new LetExpr(new VarExpr("x"), new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1)))) == true);
    CHECK(test-> equals(new LetExpr(new VarExpr("y"), new NumExpr(5), new AddExpr(new VarExpr("y"), new NumExpr(1)))) == false);
    CHECK(test-> equals(new NumExpr(6)) == false);
}

TEST_CASE("BoolExpr Equals"){
    Expr *test = new BoolExpr(true);
    CHECK((test -> equals(new BoolExpr(true))) == true);
    CHECK((test -> equals(new BoolExpr(false))) == false);
    CHECK((test -> equals(new NumExpr(3))) == false);
}

TEST_CASE("EqualExpr Equals"){
    Expr *test = new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
    CHECK((test -> equals(new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2))))) == true);
    CHECK((test -> equals(new EqualExpr(new NumExpr(2), new NumExpr(3)))) == false);
    CHECK((test -> equals(new NumExpr(3))) == false);
}

TEST_CASE("IfExpr Equals"){
    // _if 0 == -1 + 1
    // _then 1
    // _else 0
    Expr *test = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                            new NumExpr(1), new NumExpr(0));
    CHECK((test -> equals(new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                                     new NumExpr(1), new NumExpr(0)))) == true);
    CHECK((test -> equals(new NumExpr(3))) == false);
}


// INTERP TESTS

TEST_CASE("Num Interp"){
    Expr *test = new NumExpr(42);
    CHECK( (test-> interp()) -> equals (new NumVal(42)));
}

TEST_CASE("Add Interp"){
    // test == 1 + 2 + 3 == 6
    Expr *test = new AddExpr(new AddExpr(new NumExpr(1), new NumExpr(2)), new NumExpr(3));
    // test == 6
    CHECK( (test-> interp()) -> equals (new NumVal(6)));
    // throws exception
    CHECK_THROWS_WITH( (new AddExpr(new VarExpr("Wrong"), new NumExpr(1))) -> interp(),
                      "Error: Expr contains a string element." );
}

TEST_CASE("Mult Interp"){
    // test == 1 * 2 * 3 == 6
    Expr *test = new MultExpr(new MultExpr(new NumExpr(1), new NumExpr(2)), new NumExpr(3));
    // test == 6
    CHECK( (test-> interp()) -> equals (new NumVal(6)));
    // throws exception
    CHECK_THROWS_WITH( (new MultExpr(new VarExpr("Wrong"), new NumExpr(1))) -> interp(),
                      "Error: Expr contains a string element." );
}

TEST_CASE("Var Interp"){
    // test = "owls"
    Expr *test = new VarExpr("owls");
    // throws exception
    CHECK_THROWS_WITH(test -> interp(), "Error: Expr contains a string element.");
}

TEST_CASE("Let Interp"){
    // test1 =
    //        _let x = 5;
    //        _in 6;
    Expr *test1 = new LetExpr(new VarExpr("x"), new NumExpr(5), new NumExpr(6));
    CHECK( (test1 -> interp()) -> equals (new NumVal(6)));
    // test2 =
    //        _let x = 5;
    //        _in x + 1;
    Expr *test2 = new LetExpr(new VarExpr("x"),
                           new NumExpr(5),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK((test2 -> interp()) -> equals (new NumVal(6)));
    // test3 =
    //        _let x = 5;
    //        _in y + 1;
    Expr *test3 = new LetExpr(new VarExpr("x"),
                           new NumExpr(5),
                           new AddExpr(new VarExpr("y"), new NumExpr(1)));
    CHECK_THROWS_WITH( test3 -> interp(),
                      "Error: Expr contains a string element." );
    // test4 =
    //        _let x = 5;
    //        _in _let x = 3;
    //            _in x + 1;
    Expr *test4 = new LetExpr(new VarExpr("x"),
                           new NumExpr(5),
                           new LetExpr(new VarExpr("x"),
                                    new NumExpr(3),
                                    new AddExpr(new VarExpr("x"), new NumExpr(1))));
    CHECK((test4 -> interp()) -> equals (new NumVal(4)));
    // test5 =
    //        _let x = 5;
    //        _in _let y = 3;
    //            _in x + 1;
    Expr *test5 = new LetExpr(new VarExpr("x"), new NumExpr(5),
                           new LetExpr(new VarExpr("y"),
                                    new NumExpr(3),
                                    new AddExpr(new VarExpr("x"), new NumExpr(1))));
    CHECK((test5 -> interp()) -> equals (new NumVal(6)));
    // test6 =
    //        _let x = 5;
    //        _in _let x = x + 1;
    //            _in x + 1;
    Expr *test6 = new LetExpr(new VarExpr("x"), new NumExpr(5),
                           new LetExpr(new VarExpr("x"),
                                    new AddExpr(new VarExpr("x"), new NumExpr(1)),
                                    new AddExpr(new VarExpr("x"), new NumExpr(1))));
    CHECK((test6 -> interp()) -> equals (new NumVal(7)));
    // test7 =
    //        _let x = x + 1;
    //        _in x + 1;
    Expr *test7 = new LetExpr(new VarExpr("x"),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK_THROWS_WITH( test7 -> interp(),
                      "Error: Expr contains a string element." );
    // test8 =
    //        _let x = _let x = 3
    //                 _in x + 1
    //        _in x + 1
    Expr *test8 = new LetExpr(new VarExpr("x"),
                           new LetExpr(new VarExpr("x"), new NumExpr(3),
                                    new AddExpr(new VarExpr("x"), new NumExpr(1))),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK((test8 -> interp()) -> equals (new NumVal(5)));
}

TEST_CASE("BoolExpr Interp"){
    Expr *test = new BoolExpr(false);
    CHECK_THROWS_WITH((test -> interp()), "Error: Expr contains a boolean element.");
}

TEST_CASE("EqualExpr Interp"){
    Expr *test1 = new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
    CHECK((test1 -> interp()) -> equals(new BoolVal(true)) == true);
    CHECK((test1 -> interp()) -> equals(new BoolVal(false)) == false);
    Expr *test2 = new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(-2)));
    CHECK((test2 -> interp()) -> equals(new BoolVal(false)) == true);
    CHECK((test2 -> interp()) -> equals(new BoolVal(true)) == false);
}

TEST_CASE("IfExpr Interp"){
    // _if 0 == -1 + 1
    // _then 1
    // _else 0
    Expr *test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                            new NumExpr(1), new NumExpr(0));
    CHECK((test1 -> interp()) -> equals(new NumVal(1)) == true);
    CHECK((test1 -> interp()) -> equals(new NumVal(0)) == false);
    // _if 1 == -1 + 1
    // _then 1
    // _else 0
    Expr *test2 = new IfExpr(new EqualExpr(new NumExpr(1), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                            new NumExpr(1), new NumExpr(0));
    CHECK((test2 -> interp()) -> equals(new NumVal(1)) == false);
    CHECK((test2 -> interp()) -> equals(new NumVal(0)) == true);
    // if 4
    // _then 1
    // _else 0
    Expr *test3 = new IfExpr(new NumExpr(4),new NumExpr(1), new NumExpr(0));
    CHECK_THROWS_WITH(test3 -> interp(), "Error: IfExpr's condition is not a boolean value.");
}


// HAS_VARIABLE TESTS

TEST_CASE("Num Has Variable"){
    CHECK((new NumExpr(2)) -> has_variable() == false);
}

TEST_CASE("Add Has Variable"){
    Expr *test1 = new AddExpr(new AddExpr(new NumExpr(1), new NumExpr(1)), new NumExpr(1));
    CHECK((test1 -> has_variable()) == false);
    Expr *test2 = new AddExpr(new AddExpr(new NumExpr(1), new VarExpr("owls")), new NumExpr(1));
    CHECK((test2 -> has_variable()) == true);
}

TEST_CASE("Mult Has Variable"){
    Expr *test1 = new MultExpr(new AddExpr(new NumExpr(1), new NumExpr(1)), new NumExpr(1));
    CHECK((test1 -> has_variable()) == false);
    Expr *test2 = new MultExpr(new MultExpr(new NumExpr(1), new VarExpr("owls")), new NumExpr(1));
    CHECK((test2 -> has_variable()) == true);
}

TEST_CASE("Variable Has Variable"){
    CHECK((new VarExpr("owls")) -> has_variable() == true);
}

TEST_CASE("Let Has Variable"){
    // test1 =
    //          _let x = 5;
    //          _in 6;
    Expr *test1 = new LetExpr(new VarExpr("x"), new NumExpr(5), new NumExpr(6));
    CHECK(test1 -> has_variable() == false);
    // test2 =
    //          _let x = x + 1;
    //          _in 6;
    Expr *test2 = new LetExpr(new VarExpr("x"),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)),
                           new NumExpr(6));
    CHECK(test2 -> has_variable() == true);
    // test3 =
    //          _let x = 5;
    //          _in x + 1;
    Expr *test3 = new LetExpr(new VarExpr("x"),
                           new NumExpr(5),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK(test3 -> has_variable() == true);
    // test4 =
    //          _let x = x + 1;
    //          _in x + 1;
    Expr *test4 = new LetExpr(new VarExpr("x"),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK(test4 -> has_variable() == true);
}

TEST_CASE("BoolExpr Has Variable"){
    Expr *test = new BoolExpr(true);
    CHECK(test -> has_variable() == false);
}

TEST_CASE("EqualExpr Has Variable"){
    Expr *test1 = new EqualExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
    CHECK(test1 -> has_variable() == false);
    Expr *test2 = new EqualExpr(new NumExpr(3), new AddExpr(new VarExpr("1"), new NumExpr(2)));
    CHECK(test2 -> has_variable() == true);
}

TEST_CASE("IfExpr Has Variable"){
    // _if 0 == -1 + 1
    // _then 1
    // _else 0
    Expr *test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                            new NumExpr(1), new NumExpr(0));
    CHECK((test1 -> has_variable()) == false);
    // _if 1 == -1 + 1
    // _then good
    // _else bad
    Expr *test2 = new IfExpr(new EqualExpr(new NumExpr(1), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                            new VarExpr("good"), new VarExpr("bad"));
    CHECK((test2 -> has_variable()) == true);
}



// SUBST TESTS

TEST_CASE("Num Subst"){
    CHECK(((new NumExpr(2))->subst("owls", new VarExpr("snowy owls"))) ->equals((new NumExpr(2))));
    CHECK(((new NumExpr(2))->subst("owls", new NumExpr(3))) ->equals((new NumExpr(2))));
}

TEST_CASE("Add Subst"){
    // test1 contains no string
    Expr *test1 = new AddExpr(new NumExpr(3), new MultExpr(new NumExpr(1), new NumExpr(5)));
    CHECK(((test1 -> subst("owls", new VarExpr("snowy owls"))) ->equals(test1)));
    
    // test2 contains target string
    Expr *test2 = new AddExpr(new NumExpr(3), new MultExpr(new VarExpr("owls"), new NumExpr(5)));
    CHECK(((test2 -> subst("owls", new VarExpr("snowy owls"))) ->equals(
       new AddExpr(new NumExpr(3), new MultExpr(new VarExpr("snowy owls"), new NumExpr(5))))));
    
    // test3 contains string but != target
    Expr *test3 = new AddExpr(new NumExpr(3), new MultExpr(new VarExpr("barn owls"), new NumExpr(5)));
    CHECK(((test3 -> subst("owls", new VarExpr("snowy owls"))) -> equals(test3)));
}

TEST_CASE("Mult Subst"){
    // test1 contains no string
    Expr *test1 = new MultExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(5)));
    CHECK(((test1 -> subst("owls", new VarExpr("snowy owls"))) ->equals(test1)));
    
    // test2 contains target string
    Expr *test2 = new MultExpr(new NumExpr(3), new AddExpr(new VarExpr("owls"), new NumExpr(5)));
    CHECK(((test2 -> subst("owls", new VarExpr("snowy owls"))) ->equals(
       new MultExpr(new NumExpr(3), new AddExpr(new VarExpr("snowy owls"), new NumExpr(5))))));
    
    // test3 contains string but != target
    Expr *test3 = new MultExpr(new NumExpr(3), new MultExpr(new VarExpr("barn owls"), new NumExpr(5)));
    CHECK(((test3 -> subst("owls", new VarExpr("snowy owls"))) -> equals(test3)));
}

TEST_CASE("Var Subst"){
    CHECK(((new VarExpr("owls"))->subst("owls", new VarExpr("snowy owls")))
          ->equals((new VarExpr("snowy owls"))));
    CHECK(((new VarExpr("barn owls"))->subst("owls", new VarExpr("snowy owls")))
          ->equals((new VarExpr("barn owls"))));
}

TEST_CASE("Let Subst"){
    // test1 =
    //          _let x = 5;
    //          _in 6;          -> subst ("x", 1)
    Expr *test1 = new LetExpr(new VarExpr("x"), new NumExpr(5), new NumExpr(6));
    CHECK((test1 -> subst("x", new NumExpr(1))) -> equals(test1));
    // test2 =
    //          _let x = x + 1;
    //          _in x + 1;          -> subst ("x", 5)
    Expr *test2 = new LetExpr(new VarExpr("x"),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK(test2 -> subst("x", new NumExpr(5)) -> equals(
          new LetExpr(new VarExpr("x"),
                   new AddExpr(new NumExpr(5), new NumExpr(1)),
                   new AddExpr(new VarExpr("x"), new NumExpr(1)))));
    // test3 =
    //          _let x = x + 1;
    //          _in x + 1;          -> subst ("y", 5)
    Expr *test3 = new LetExpr(new VarExpr("x"),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK(test3 -> subst("y", new NumExpr(5)) -> equals(test3));
}

TEST_CASE("BoolExpr Subst"){
    CHECK(((new BoolExpr(true))->subst("owls", new VarExpr("snowy owls"))) ->equals((new BoolExpr(true))));
    CHECK(((new BoolExpr(false))->subst("owls", new NumExpr(3))) ->equals((new BoolExpr(false))));
}

TEST_CASE("EqualExpr Subst"){
    Expr *test = new EqualExpr(new VarExpr("grade"), new VarExpr("finalGPA"));
    CHECK((test -> subst("finalGPA", new NumExpr(4))) -> equals
          (new EqualExpr(new VarExpr("grade"), new NumExpr(4))) == true);
    CHECK((test -> subst("finalGPA", new NumExpr(4))) -> equals
          (new EqualExpr(new NumExpr(4), new VarExpr("finalGPA"))) == false);
}

TEST_CASE("IfExpr Subst"){
    // _if 0 == -1 + 1
    // _then 1
    // _else 0
    Expr *test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                            new NumExpr(1), new NumExpr(0));
    CHECK((test1 -> subst("good", new NumExpr(42))) -> equals(test1) == true);
    // _if 0 == -1 + 1
    // _then good
    // _else 0
    Expr *test2 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                            new VarExpr("good"), new NumExpr(0));
    CHECK((test2 -> subst("good", new NumExpr(1))) -> equals(test1) == true);
}

// PRINT TESTS

TEST_CASE("Num Print"){
    Expr *test1 = new NumExpr(10);
    CHECK((test1 -> to_string()) == "10");
}

TEST_CASE("Add Print"){
    Expr *test1 = new AddExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
    CHECK((test1 -> to_string()) == "(3+(1+2))");
    Expr *test2 = new AddExpr(new AddExpr(new NumExpr(3), new NumExpr(1)), new NumExpr(2));
    CHECK((test2 -> to_string()) == "((3+1)+2)");
}

TEST_CASE("Mult Print"){
    Expr *test1 = new MultExpr(new NumExpr(3), new MultExpr(new NumExpr(1), new NumExpr(2)));
    CHECK((test1 -> to_string()) == "(3*(1*2))");
    Expr *test2 = new MultExpr(new MultExpr(new NumExpr(3), new NumExpr(1)), new NumExpr(2));
    CHECK((test2 -> to_string()) == "((3*1)*2)");
}

TEST_CASE("Variable Print"){
    Expr *test1 = new VarExpr("X");
    CHECK((test1 -> to_string()) == "X");
}

TEST_CASE("Let Print"){
    // test1 =
    //        _let x = 5;
    //        _in x + 1;
    Expr *test1 = new LetExpr(new VarExpr("x"),
                           new NumExpr(5),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK((test1 -> to_string()) == "(_let x=5 _in (x+1))");
    // test2 =
    //        _let x = 5;
    //        _in _let x = 3;
    //            _in x + 1;
    Expr *test2 = new LetExpr(new VarExpr("x"),
                           new NumExpr(5),
                           new LetExpr(new VarExpr("x"),
                                    new NumExpr(3),
                                    new AddExpr(new VarExpr("x"), new NumExpr(1))));
    CHECK((test2 -> to_string()) == "(_let x=5 _in (_let x=3 _in (x+1)))");
    // test3 =
    //        _let x = x + 1;
    //        _in x + 1;
    Expr *test3 = new LetExpr(new VarExpr("x"),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK((test3 -> to_string()) == "(_let x=(x+1) _in (x+1))");
}

TEST_CASE("boolExpr Print"){
    Expr *test1 = new BoolExpr(true);
    CHECK((test1 -> to_string()) == "_true");
    Expr *test2 = new BoolExpr(false);
    CHECK((test2 -> to_string()) == "_false");
}

TEST_CASE("EqualExpr Print"){
    Expr *test1 = new EqualExpr(new VarExpr("grade"), new NumExpr(99));
    CHECK((test1 -> to_string()) == "(grade == 99)");
    Expr *test2 = new EqualExpr(new VarExpr("grade"), new AddExpr(new NumExpr(30), new NumExpr(60)));
    CHECK((test2 -> to_string()) == "(grade == (30+60))");
}

TEST_CASE("IfExpr Print"){
    // _if 0 == -1 + 1
    // _then 1
    // _else 0
    Expr *test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                            new NumExpr(1), new NumExpr(0));
    CHECK((test1 -> to_string()) == "(_if ((0 == (-1+1))) _then (1) _else (0))");
    // if good
    // _then 1
    // _else 0
    Expr *test2 = new IfExpr(new VarExpr("good"),new NumExpr(1), new NumExpr(0));
    CHECK((test2 -> to_string()) == "(_if (good) _then (1) _else (0))");
}

// PRETTY_PRINT TESTS

TEST_CASE("Num Pretty Print"){
    Expr *test1 = new NumExpr(10);
    CHECK((test1 -> to_pretty_string()) == "10");
}

TEST_CASE("Add Pretty Print"){
    Expr *test1 = new AddExpr(new NumExpr(3), new AddExpr(new NumExpr(1), new NumExpr(2)));
    CHECK((test1 -> to_pretty_string()) == "3 + 1 + 2");
    Expr *test2 = new AddExpr(new AddExpr(new NumExpr(3), new NumExpr(1)), new NumExpr(2));
    CHECK((test2 -> to_pretty_string()) == "(3 + 1) + 2");
}

TEST_CASE("Mult Pretty Print"){
    Expr *test1 = new MultExpr(new NumExpr(3), new MultExpr(new NumExpr(1), new NumExpr(2)));
    CHECK((test1 -> to_pretty_string()) == "3 * 1 * 2");
    Expr *test2 = new MultExpr(new MultExpr(new NumExpr(3), new NumExpr(1)), new NumExpr(2));
    CHECK((test2 -> to_pretty_string()) == "(3 * 1) * 2");
}

TEST_CASE("Add and Mult Mixed Pretty Print"){
    // 1 + 2 * 3
    Expr *test1 = new AddExpr(new NumExpr(1), new MultExpr(new NumExpr(2), new NumExpr(3)));
    CHECK((test1 -> to_pretty_string()) == "1 + 2 * 3");
    // 1 * (2 + 3)
    Expr *test2 = new MultExpr(new NumExpr(1), new AddExpr(new NumExpr(2), new NumExpr(3)));
    CHECK((test2 -> to_pretty_string()) == "1 * (2 + 3)");
    // 1 * (2 + 3) * 4
    Expr *test3 = new MultExpr(new NumExpr(1), new MultExpr(new AddExpr(new NumExpr(2), new NumExpr(3)), new NumExpr(4)));
    CHECK((test3 -> to_pretty_string()) == "1 * (2 + 3) * 4");
    // (1 + 2) * (3 + 4)
    Expr *test4 = new MultExpr(new AddExpr(new NumExpr(1), new NumExpr(2)), new AddExpr(new NumExpr(3), new NumExpr(4)));
    CHECK((test4 -> to_pretty_string()) == "(1 + 2) * (3 + 4)");
    // (1 * (2 + 3)) * 4
    Expr *test5 = new MultExpr(new MultExpr(new NumExpr(1), new AddExpr(new NumExpr(2), new NumExpr(3))), new NumExpr(4));
    CHECK((test5 -> to_pretty_string()) == "(1 * (2 + 3)) * 4");
}

TEST_CASE("Var Pretty Print"){
    Expr *test1 = new VarExpr("Y");
    CHECK((test1 -> to_pretty_string()) == "Y");
}

TEST_CASE("_let Pretty Print"){
    // test1 =
    //        _let x = 5
    //        _in x + 1
    Expr *test1 = new LetExpr(new VarExpr("x"),
                           new NumExpr(5),
                           new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK((test1 -> to_pretty_string()) == "_let x = 5\n_in x + 1");
    // test2 =
    //        (_let x = 5
    //        _in x) + 1
    Expr *test2 = new AddExpr(new LetExpr(new VarExpr("x"), new NumExpr(5), new VarExpr("x")),
                          new NumExpr(1));
    CHECK((test2 -> to_pretty_string()) == "(_let x = 5\n _in x) + 1");
    // test3 =
    //        5 * (_let x = 5
    //             _in  x) + 1
    Expr *test3 = new AddExpr (new MultExpr(new NumExpr(5),
                                    new LetExpr(new VarExpr("x"), new NumExpr(5), new VarExpr("x"))),
                           new NumExpr(1));
    CHECK((test3 -> to_pretty_string()) == "5 * (_let x = 5\n"
                                           "     _in x) + 1");
    // test4 =
    //        5 * _let x = 5
    //            _in  x + 1
    Expr *test4 = new MultExpr(new NumExpr(5),
                           new LetExpr(new VarExpr("x"),
                                    new NumExpr(5),
                                    new AddExpr(new VarExpr("x"), new NumExpr(1))));
    CHECK((test4 -> to_pretty_string()) == "5 * _let x = 5\n"
                                           "    _in x + 1");
    // test5 =
    //        _let x = 5
    //        _in _let x = 3
    //            _in x + 1
    Expr *test5 = new LetExpr(new VarExpr("x"),
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
    Expr *test6 = new LetExpr(new VarExpr("x"),
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
    Expr *test7 = new MultExpr(new AddExpr(new NumExpr(5),
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

TEST_CASE("boolExpr Pretty Print"){
    Expr *test1 = new BoolExpr(true);
    CHECK((test1 -> to_pretty_string()) == "_true");
    Expr *test2 = new BoolExpr(false);
    CHECK((test2 -> to_pretty_string()) == "_false");
}

TEST_CASE("EqualExpr Pretty Print"){
    Expr *test1 = new EqualExpr(new VarExpr("grade"), new NumExpr(99));
    CHECK((test1 -> to_pretty_string()) == "grade == 99");
    Expr *test2 = new EqualExpr(new VarExpr("grade"), new AddExpr(new NumExpr(10), new NumExpr(80)));
    CHECK((test2 -> to_pretty_string()) == "grade == 10 + 80");
    Expr *test3 = new AddExpr(new EqualExpr(new NumExpr(1), new VarExpr("ice-cream")), new NumExpr(-3));
    CHECK((test3 -> to_pretty_string()) == "(1 == ice-cream) + -3");
}

TEST_CASE("IfExpr Pretty Print"){
    // _if 0 == -1 + 1
    // _then 1
    // _else 0
    Expr *test1 = new IfExpr(new EqualExpr(new NumExpr(0), new AddExpr(new NumExpr(-1), new NumExpr(1))),
                            new NumExpr(1), new NumExpr(0));
    CHECK((test1 -> to_pretty_string()) == "_if 0 == -1 + 1\n"
                                           "_then 1\n"
                                           "_else 0");
    // if good
    // _then 1
    // _else 0
    Expr *test2 = new IfExpr(new VarExpr("good"),new NumExpr(1), new NumExpr(0));
    CHECK((test2 -> to_pretty_string()) == "_if good\n"
                                           "_then 1\n"
                                           "_else 0");
}



// PARSE TESTS

TEST_CASE("Parse Num"){
    CHECK(parse("3234") -> equals (new NumExpr(3234)));
    CHECK(parse(" 211") -> equals (new NumExpr(211)));
    CHECK(parse("-19 ") -> equals (new NumExpr(-19)));
    CHECK(parse("( -3    )") -> equals (new NumExpr(-3)));
    CHECK_THROWS_WITH(parse("(99"), "missing close parentheses");
}

TEST_CASE("Parse Add"){
    CHECK(parse("2 + 1") -> equals (new AddExpr(new NumExpr(2), new NumExpr(1))));
    CHECK(parse("-7 + 6") -> equals (new AddExpr(new NumExpr(-7), new NumExpr(6))));
    CHECK(parse("(3 + 2)") -> equals (new AddExpr(new NumExpr(3), new NumExpr(2))));
    CHECK(parse("   5+1") -> equals (new AddExpr(new NumExpr(5), new NumExpr(1))));
    CHECK_THROWS_WITH(parse("(9 +"), "invalid input (parse_multicand())");
    CHECK_THROWS_WITH(parse("(9 +1"), "missing close parentheses");
    CHECK_THROWS_WITH(parse("9 +)"), "invalid input (parse_multicand())");
}

TEST_CASE("Parse Mult"){
    CHECK(parse("6 * 12") -> equals (new MultExpr(new NumExpr(6), new NumExpr(12))));
    CHECK(parse("-1*2") -> equals (new MultExpr(new NumExpr(-1), new NumExpr(2))));
    CHECK(parse("(-8)*  4") -> equals (new MultExpr(new NumExpr(-8), new NumExpr(4))));
    CHECK(parse("(2  * 1)") -> equals (new MultExpr(new NumExpr(2), new NumExpr(1))));
    CHECK_THROWS_WITH(parse("(2  * 1"), "missing close parentheses");
    CHECK_THROWS_WITH(parse("2  * 1)"), "missing open parentheses");
    CHECK_THROWS_WITH(parse("(((((2  * 1)"), "missing close parentheses");
    CHECK_THROWS_WITH(parse("(2  * 1))))))"), "missing open parentheses");
}

TEST_CASE("Parse Var"){
    CHECK(parse("cat") -> equals (new VarExpr("cat")));
    CHECK(parse("  dog") -> equals (new VarExpr("dog")));
    CHECK(parse("OWLS") -> equals (new VarExpr("OWLS")));
    CHECK(parse("wild_animals") -> equals (new VarExpr("wild_animals")));
    CHECK_THROWS_WITH(parse("wha.le"), "invalid input (parse_var())");
}

TEST_CASE("Parse Bool"){
    CHECK(parse("_true") -> equals(new BoolExpr(true)));
    CHECK(parse("_false") -> equals(new BoolExpr(false)));
    CHECK_THROWS_WITH(parse("_t"), "invalid input (parse_bool())");
    CHECK_THROWS_WITH(parse("_fals 1"), "invalid input (parse_bool(), '_false')");
}

//TEST_CASE("Parse Equal"){
//
//}

TEST_CASE("Parse _let"){
    CHECK(parse("_let x = 5 _in x+2") -> equals
          (new LetExpr(new VarExpr("x"), new NumExpr(5),
                    new AddExpr(new VarExpr("x"), new NumExpr(2)))));
    CHECK(parse("_let x = (x+2) _in      (x+-3)") -> equals
          (new LetExpr(new VarExpr("x"), new AddExpr(new VarExpr("x"), new NumExpr(2)),
                    new AddExpr(new VarExpr("x"), new NumExpr(-3)))));
    CHECK_THROWS_WITH(parse("_let x = 1    _i"), "invalid input (parse_let(), '_in')");
}


TEST_CASE("Parse _if"){
    CHECK(parse("_if _true _then 1 _else 0") -> equals(new IfExpr(new BoolExpr(true), new NumExpr(1), new NumExpr(0))));
    CHECK(parse("_if 3=2+1 _then a _else b") -> equals(new IfExpr(
                                                            new EqualExpr(new NumExpr(3),
                                                                          new AddExpr(new NumExpr(2), new NumExpr(1))),
                                                            new VarExpr("a"),
                                                            new VarExpr("b"))));
    CHECK_THROWS_WITH(parse("_if x = 1 then 2 else 1"), "invalid input (parse_if(), '_then')");
}


TEST_CASE("Mixed Parse"){
    CHECK(parse("6 + (2 * -7)") -> equals(new AddExpr(new NumExpr(6),
                                                  new MultExpr(new NumExpr(2), new NumExpr(-7)))));
    CHECK(parse("(-3)  +  4 * (_let x = 2 _in x+1)") -> equals(
          new AddExpr(new NumExpr(-3), new MultExpr(new NumExpr(4),
                                        new LetExpr(new VarExpr("x"), new NumExpr(2),
                                                 new AddExpr(new VarExpr("x"), new NumExpr(1)))))));
    CHECK(parse("(1234*((_letx=1_inx+-2)+7))") -> equals(
          new MultExpr(new NumExpr(1234),
                   new AddExpr(new LetExpr(new VarExpr("x"), new NumExpr(1),
                                    new AddExpr(new VarExpr("x"), new NumExpr(-2))),
                           new NumExpr(7)))));
}


TEST_CASE("NumVal Tests"){
    
    SECTION("NumVal equals()"){
        Val *test = new NumVal(3);
        CHECK(test -> equals (new NumVal(3)) == true);
        CHECK(test -> equals (new NumVal(5)) == false);
        CHECK(test -> equals ((new AddExpr(new NumExpr(1), new NumExpr(2))) -> interp()) == true);
        CHECK(test -> equals ((new MultExpr(new NumExpr(1), new NumExpr(2))) -> interp()) == false);
    }
    
    SECTION("NumVal toExpr()"){
        Val *test = new NumVal(-7);
        CHECK(((test -> to_expr()) -> equals(new NumExpr(-7))) == true);
        CHECK(((test -> to_expr()) -> equals(new NumExpr(7))) == false);
    }
    
    SECTION("NumVal to_string()"){
        Val *test = new NumVal(1010);
        CHECK(((test -> to_string()) == "1010") == true);
        CHECK(((test -> to_string()) == "0101") == false);
    }
    
    SECTION("NumVal add_to()"){
        Val *test1 = new NumVal(-5);
        Val *test2 = new NumVal(5);
        CHECK(((test1 -> add_to(test2)) -> equals (new NumVal(0))) == true);
        CHECK(((test1 -> add_to(test2)) -> equals (new NumVal(5))) == false);
        CHECK_THROWS_WITH(((test1 -> add_to(new BoolVal(true)))), "Error: rhs of Val add_to() must be a NumVal.");
    }
    
    SECTION("NumVal mult_to()"){
        Val *test1 = new NumVal(-5);
        Val *test2 = new NumVal(5);
        CHECK(((test1 -> mult_by(test2)) -> equals (new NumVal(-25))) == true);
        CHECK(((test1 -> mult_by(test2)) -> equals (new NumVal(25))) == false);
        CHECK_THROWS_WITH(((test1 -> mult_by(new BoolVal(false)))), "Error: rhs of Val mult_by() must be a NumVal.");
    }
}

TEST_CASE("BoolVal Tests"){

    SECTION("BoolVal equals()"){
        Val *test = new BoolVal(true);
        CHECK(test -> equals (new BoolVal(true)) == true);
        CHECK(test -> equals (new BoolVal(false)) == false);
        CHECK(test -> equals (new NumVal(1)) == false);
    }
    
    SECTION("BoolVal toExpr()"){
        Val *test = new BoolVal(true);
        CHECK(((test -> to_expr()) == NULL) == true);
    }
    
    SECTION("BoolVal to_string()"){
        Val *test1 = new BoolVal(true);
        CHECK(((test1 -> to_string()) == "_true") == true);
        CHECK(((test1 -> to_string()) == "_false") == false);
        Val *test2 = new BoolVal(false);
        CHECK(((test2 -> to_string()) == "_false") == true);
        CHECK(((test2 -> to_string()) == "_true") == false);
    }
    
    SECTION("BoolVal add_to()"){
        Val *test = new BoolVal(true);
        CHECK_THROWS_WITH((test -> add_to(new NumVal(1))), "Error: BoolVal cannot be added.");
        CHECK_THROWS_WITH((test -> add_to(new BoolVal(true))), "Error: BoolVal cannot be added.");
    }
    
    SECTION("BoolVal mult_to()"){
        Val *test = new BoolVal(true);
        CHECK_THROWS_WITH((test -> mult_by(new NumVal(1))), "Error: BoolVal cannot be multiplied.");
        CHECK_THROWS_WITH((test -> mult_by(new BoolVal(true))), "Error: BoolVal cannot be multiplied.");
    }
}
