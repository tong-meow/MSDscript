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

TEST_CASE("_let Equals"){
    // _let x = 5;
    // _in x + 1;
    LetExpr *test = new LetExpr(new VarExpr("x"), new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1)));
    CHECK(test-> equals(new LetExpr(new VarExpr("x"), new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1)))) == true);
    CHECK(test-> equals(new LetExpr(new VarExpr("y"), new NumExpr(5), new AddExpr(new VarExpr("y"), new NumExpr(1)))) == false);
    CHECK(test-> equals(new NumExpr(6)) == false);
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

TEST_CASE("_let Interp"){
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

TEST_CASE("_let Has Variable"){
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

TEST_CASE("_let Subst"){
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

TEST_CASE("_let Print"){
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
    CHECK_THROWS_WITH(parse("(9 +"), "invalid input");
    CHECK_THROWS_WITH(parse("(9 +1"), "missing close parentheses");
    CHECK_THROWS_WITH(parse("9 +)"), "invalid input");
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
    CHECK_THROWS_WITH(parse("wha.le"), "invalid input");
}

TEST_CASE("Parse _let"){
    CHECK(parse("_let x = 5 _in x+2") -> equals
          (new LetExpr(new VarExpr("x"), new NumExpr(5),
                    new AddExpr(new VarExpr("x"), new NumExpr(2)))));
    CHECK(parse("_let x = (x+2) _in      (x+-3)") -> equals
          (new LetExpr(new VarExpr("x"), new AddExpr(new VarExpr("x"), new NumExpr(2)),
                    new AddExpr(new VarExpr("x"), new NumExpr(-3)))));
    CHECK_THROWS_WITH(parse("_let x = 1    _i"), "invalid input");
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

/*
To full test a method like NumVal::equals, you'll need a pointer of type Val* that is not a pointer to a NumVal object —
 but NumVal is the only instantiable Val class at first!
 To work around that problem, use nullptr in tests for now (even though that's usually a bad idea),
 since nullptr can have type Val* and is not a pointer to a NumVal object.
 */
TEST_CASE("Val Tests"){
    
    SECTION("Val equals()"){
        Val *test = new NumVal(3);
        CHECK(test -> equals (new NumVal(3)) == true);
        CHECK(test -> equals (new NumVal(5)) == false);
        CHECK(test -> equals ((new AddExpr(new NumExpr(1), new NumExpr(2))) -> interp()) == true);
        CHECK(test -> equals ((new MultExpr(new NumExpr(1), new NumExpr(2))) -> interp()) == false);
    }
    
    SECTION("Val toExpr()"){
        Val *test = new NumVal(-7);
        CHECK(((test -> to_expr()) -> equals(new NumExpr(-7))) == true);
        CHECK(((test -> to_expr()) -> equals(new NumExpr(7))) == false);
    }
    
    SECTION("Val to_string()"){
        Val *test = new NumVal(1010);
        CHECK(((test -> to_string()) == "1010") == true);
        CHECK(((test -> to_string()) == "0101") == false);
    }
    
    SECTION("Val add_to()"){
        Val *test1 = new NumVal(-5);
        Val *test2 = new NumVal(5);
        CHECK(((test1 -> add_to(test2)) -> equals (new NumVal(0))) == true);
        CHECK(((test1 -> add_to(test2)) -> equals (new NumVal(5))) == false);
    }
    
    SECTION("Val mult_to()"){
        Val *test1 = new NumVal(-5);
        Val *test2 = new NumVal(5);
        CHECK(((test1 -> mult_by(test2)) -> equals (new NumVal(-25))) == true);
        CHECK(((test1 -> mult_by(test2)) -> equals (new NumVal(25))) == false);
    }
}
