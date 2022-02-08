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
#include "tests.hpp"

// EQUAL TESTS

TEST_CASE("Num Equals"){
    CHECK((new Num(2))-> equals(new Num(2)) == true);
    CHECK((new Num(2))-> equals(new Num(5)) == false);
    CHECK((new Num(2))-> equals(new Add(new Num(2), new Num(5))) == false);
}

TEST_CASE("Add Equals"){
    Expr *test = new Add(new Num(2), new Num(5));
    CHECK(test-> equals(new Add(new Num(2), new Num(5))) == true);
    CHECK(test-> equals(new Add(new Num(5), new Num(2))) == false);
    CHECK(test-> equals(new Add(new Num(4), new Num(10))) == false);
    CHECK(test-> equals(new Mult(new Num(3), new Num(6))) == false);
}

TEST_CASE("Mult Equals"){
    Expr *test = new Mult(new Num(3), new Num(6));
    CHECK(test-> equals(new Mult(new Num(3), new Num(6))) == true);
    CHECK(test-> equals(new Mult(new Num(6), new Num(3))) == false);
    CHECK(test-> equals(new Mult(new Num(1), new Num(2))) == false);
    CHECK(test-> equals(new Add(new Num(2), new Num(5))) == false);
}

TEST_CASE("Var Equals"){
    Expr *test = new Var("Happy2022");
    CHECK(test-> equals(new Var("Happy2022")) == true);
    CHECK(test-> equals(new Var("PlayStation")) == false);
    CHECK(test-> equals(new Var("XBox")) == false);
    CHECK(test-> equals(new Mult(new Num(1), new Num(2))) == false);
}

TEST_CASE("_let Equals"){
    // _let x = 5;
    // _in x + 1;
    _let *test = new _let(new Var("x"), new Num(5), new Add(new Var("x"), new Num(1)));
    CHECK(test-> equals(new _let(new Var("x"), new Num(5), new Add(new Var("x"), new Num(1)))) == true);
    CHECK(test-> equals(new _let(new Var("y"), new Num(5), new Add(new Var("y"), new Num(1)))) == false);
    CHECK(test-> equals(new Num(6)) == false);
}


// INTERP TESTS

TEST_CASE("Num Interp"){
    Expr *test = new Num(42);
    CHECK((test-> interp()) == 42);
}

TEST_CASE("Add Interp"){
    // test == 1 + 2 + 3 == 6
    Expr *test = new Add(new Add(new Num(1), new Num(2)), new Num(3));
    // test == 6
    CHECK((test-> interp()) == 6);
    // throws exception
    CHECK_THROWS_WITH( (new Add(new Var("Wrong"), new Num(1))) -> interp(),
                      "Error: Expr contains a string element." );
}

TEST_CASE("Mult Interp"){
    // test == 1 * 2 * 3 == 6
    Expr *test = new Mult(new Mult(new Num(1), new Num(2)), new Num(3));
    // test == 6
    CHECK((test-> interp()) == 6);
    // throws exception
    CHECK_THROWS_WITH( (new Mult(new Var("Wrong"), new Num(1))) -> interp(),
                      "Error: Expr contains a string element." );
}

TEST_CASE("Var Interp"){
    // test = "owls"
    Expr *test = new Var("owls");
    // throws exception
    CHECK_THROWS_WITH(test -> interp(), "Error: Expr contains a string element.");
}

TEST_CASE("_let Interp"){
    // test1 =
    //        _let x = 5;
    //        _in 6;
    Expr *test1 = new _let(new Var("x"), new Num(5), new Num(6));
    CHECK( test1 -> interp() == 6);
    // test2 =
    //        _let x = 5;
    //        _in x + 1;
    Expr *test2 = new _let(new Var("x"),
                           new Num(5),
                           new Add(new Var("x"), new Num(1)));
    CHECK((test2 -> interp()) == 6);
    // test3 =
    //        _let x = 5;
    //        _in y + 1;
    Expr *test3 = new _let(new Var("x"),
                           new Num(5),
                           new Add(new Var("y"), new Num(1)));
    CHECK_THROWS_WITH( test3 -> interp(),
                      "Error: Expr contains a string element." );
    // test4 =
    //        _let x = 5;
    //        _in _let x = 3;
    //            _in x + 1;
    Expr *test4 = new _let(new Var("x"),
                           new Num(5),
                           new _let(new Var("x"),
                                    new Num(3),
                                    new Add(new Var("x"), new Num(1))));
    CHECK(test4 -> interp() == 4);
    // test5 =
    //        _let x = 5;
    //        _in _let y = 3;
    //            _in x + 1;
    Expr *test5 = new _let(new Var("x"), new Num(5),
                           new _let(new Var("y"),
                                    new Num(3),
                                    new Add(new Var("x"), new Num(1))));
    CHECK((test5 -> interp()) == 6);
    // test6 =
    //        _let x = 5;
    //        _in _let x = x + 1;
    //            _in x + 1;
    Expr *test6 = new _let(new Var("x"), new Num(5),
                           new _let(new Var("x"),
                                    new Add(new Var("x"), new Num(1)),
                                    new Add(new Var("x"), new Num(1))));
    CHECK((test6 -> interp()) == 7);
    // test7 =
    //        _let x = x + 1;
    //        _in x + 1;
    Expr *test7 = new _let(new Var("x"),
                           new Add(new Var("x"), new Num(1)),
                           new Add(new Var("x"), new Num(1)));
    CHECK_THROWS_WITH( test7 -> interp(),
                      "Error: Expr contains a string element." );
    // test8 =
    //        _let x = _let x = 3
    //                 _in x + 1
    //        _in x + 1
    Expr *test8 = new _let(new Var("x"),
                           new _let(new Var("x"), new Num(3),
                                    new Add(new Var("x"), new Num(1))),
                           new Add(new Var("x"), new Num(1)));
    CHECK((test8 -> interp()) == 5);
}



// HAS_VARIABLE TESTS

TEST_CASE("Num Has Variable"){
    CHECK((new Num(2)) -> has_variable() == false);
}

TEST_CASE("Add Has Variable"){
    Expr *test1 = new Add(new Add(new Num(1), new Num(1)), new Num(1));
    CHECK((test1 -> has_variable()) == false);
    Expr *test2 = new Add(new Add(new Num(1), new Var("owls")), new Num(1));
    CHECK((test2 -> has_variable()) == true);
}

TEST_CASE("Mult Has Variable"){
    Expr *test1 = new Mult(new Add(new Num(1), new Num(1)), new Num(1));
    CHECK((test1 -> has_variable()) == false);
    Expr *test2 = new Mult(new Mult(new Num(1), new Var("owls")), new Num(1));
    CHECK((test2 -> has_variable()) == true);
}

TEST_CASE("Variable Has Variable"){
    CHECK((new Var("owls")) -> has_variable() == true);
}

TEST_CASE("_let Has Variable"){
    // test1 =
    //          _let x = 5;
    //          _in 6;
    Expr *test1 = new _let(new Var("x"), new Num(5), new Num(6));
    CHECK(test1 -> has_variable() == false);
    // test2 =
    //          _let x = x + 1;
    //          _in 6;
    Expr *test2 = new _let(new Var("x"),
                           new Add(new Var("x"), new Num(1)),
                           new Num(6));
    CHECK(test2 -> has_variable() == true);
    // test3 =
    //          _let x = 5;
    //          _in x + 1;
    Expr *test3 = new _let(new Var("x"),
                           new Num(5),
                           new Add(new Var("x"), new Num(1)));
    CHECK(test3 -> has_variable() == true);
    // test4 =
    //          _let x = x + 1;
    //          _in x + 1;
    Expr *test4 = new _let(new Var("x"),
                           new Add(new Var("x"), new Num(1)),
                           new Add(new Var("x"), new Num(1)));
    CHECK(test4 -> has_variable() == true);
}



// SUBST TESTS

TEST_CASE("Num Subst"){
    CHECK(((new Num(2))->subst("owls", new Var("snowy owls"))) ->equals((new Num(2))));
    CHECK(((new Num(2))->subst("owls", new Num(3))) ->equals((new Num(2))));
}

TEST_CASE("Add Subst"){
    // test1 contains no string
    Expr *test1 = new Add(new Num(3), new Mult(new Num(1), new Num(5)));
    CHECK(((test1 -> subst("owls", new Var("snowy owls"))) ->equals(test1)));
    
    // test2 contains target string
    Expr *test2 = new Add(new Num(3), new Mult(new Var("owls"), new Num(5)));
    CHECK(((test2 -> subst("owls", new Var("snowy owls"))) ->equals(
       new Add(new Num(3), new Mult(new Var("snowy owls"), new Num(5))))));
    
    // test3 contains string but != target
    Expr *test3 = new Add(new Num(3), new Mult(new Var("barn owls"), new Num(5)));
    CHECK(((test3 -> subst("owls", new Var("snowy owls"))) -> equals(test3)));
}

TEST_CASE("Mult Subst"){
    // test1 contains no string
    Expr *test1 = new Mult(new Num(3), new Add(new Num(1), new Num(5)));
    CHECK(((test1 -> subst("owls", new Var("snowy owls"))) ->equals(test1)));
    
    // test2 contains target string
    Expr *test2 = new Mult(new Num(3), new Add(new Var("owls"), new Num(5)));
    CHECK(((test2 -> subst("owls", new Var("snowy owls"))) ->equals(
       new Mult(new Num(3), new Add(new Var("snowy owls"), new Num(5))))));
    
    // test3 contains string but != target
    Expr *test3 = new Mult(new Num(3), new Mult(new Var("barn owls"), new Num(5)));
    CHECK(((test3 -> subst("owls", new Var("snowy owls"))) -> equals(test3)));
}

TEST_CASE("Var Subst"){
    CHECK(((new Var("owls"))->subst("owls", new Var("snowy owls")))
          ->equals((new Var("snowy owls"))));
    CHECK(((new Var("barn owls"))->subst("owls", new Var("snowy owls")))
          ->equals((new Var("barn owls"))));
}

TEST_CASE("_let Subst"){
    // test1 =
    //          _let x = 5;
    //          _in 6;          -> subst ("x", 1)
    Expr *test1 = new _let(new Var("x"), new Num(5), new Num(6));
    CHECK((test1 -> subst("x", new Num(1))) -> equals(test1));
    // test2 =
    //          _let x = x + 1;
    //          _in x + 1;          -> subst ("x", 5)
    Expr *test2 = new _let(new Var("x"),
                           new Add(new Var("x"), new Num(1)),
                           new Add(new Var("x"), new Num(1)));
    CHECK(test2 -> subst("x", new Num(5)) -> equals(
          new _let(new Var("x"),
                   new Add(new Num(5), new Num(1)),
                   new Add(new Var("x"), new Num(1)))));
    // test3 =
    //          _let x = x + 1;
    //          _in x + 1;          -> subst ("y", 5)
    Expr *test3 = new _let(new Var("x"),
                           new Add(new Var("x"), new Num(1)),
                           new Add(new Var("x"), new Num(1)));
    CHECK(test3 -> subst("y", new Num(5)) -> equals(test3));
}


// PRINT TESTS

TEST_CASE("Num Print"){
    Expr *test1 = new Num(10);
    CHECK((test1 -> to_string()) == "10");
}

TEST_CASE("Add Print"){
    Expr *test1 = new Add(new Num(3), new Add(new Num(1), new Num(2)));
    CHECK((test1 -> to_string()) == "(3+(1+2))");
    Expr *test2 = new Add(new Add(new Num(3), new Num(1)), new Num(2));
    CHECK((test2 -> to_string()) == "((3+1)+2)");
}

TEST_CASE("Mult Print"){
    Expr *test1 = new Mult(new Num(3), new Mult(new Num(1), new Num(2)));
    CHECK((test1 -> to_string()) == "(3*(1*2))");
    Expr *test2 = new Mult(new Mult(new Num(3), new Num(1)), new Num(2));
    CHECK((test2 -> to_string()) == "((3*1)*2)");
}

TEST_CASE("Variable Print"){
    Expr *test1 = new Var("X");
    CHECK((test1 -> to_string()) == "X");
}

TEST_CASE("_let Print"){
    // test1 =
    //        _let x = 5;
    //        _in x + 1;
    Expr *test1 = new _let(new Var("x"),
                           new Num(5),
                           new Add(new Var("x"), new Num(1)));
    CHECK((test1 -> to_string()) == "(_let x=5 _in (x+1))");
    // test2 =
    //        _let x = 5;
    //        _in _let x = 3;
    //            _in x + 1;
    Expr *test2 = new _let(new Var("x"),
                           new Num(5),
                           new _let(new Var("x"),
                                    new Num(3),
                                    new Add(new Var("x"), new Num(1))));
    CHECK((test2 -> to_string()) == "(_let x=5 _in (_let x=3 _in (x+1)))");
    // test3 =
    //        _let x = x + 1;
    //        _in x + 1;
    Expr *test3 = new _let(new Var("x"),
                           new Add(new Var("x"), new Num(1)),
                           new Add(new Var("x"), new Num(1)));
    CHECK((test3 -> to_string()) == "(_let x=(x+1) _in (x+1))");
}

// PRETTY_PRINT TESTS

TEST_CASE("Num Pretty Print"){
    Expr *test1 = new Num(10);
    CHECK((test1 -> to_pretty_string()) == "10");
}

TEST_CASE("Add Pretty Print"){
    Expr *test1 = new Add(new Num(3), new Add(new Num(1), new Num(2)));
    CHECK((test1 -> to_pretty_string()) == "3 + 1 + 2");
    Expr *test2 = new Add(new Add(new Num(3), new Num(1)), new Num(2));
    CHECK((test2 -> to_pretty_string()) == "(3 + 1) + 2");
}

TEST_CASE("Mult Pretty Print"){
    Expr *test1 = new Mult(new Num(3), new Mult(new Num(1), new Num(2)));
    CHECK((test1 -> to_pretty_string()) == "3 * 1 * 2");
    Expr *test2 = new Mult(new Mult(new Num(3), new Num(1)), new Num(2));
    CHECK((test2 -> to_pretty_string()) == "(3 * 1) * 2");
}

TEST_CASE("Add and Mult Mixed Pretty Print"){
    // 1 + 2 * 3
    Expr *test1 = new Add(new Num(1), new Mult(new Num(2), new Num(3)));
    CHECK((test1 -> to_pretty_string()) == "1 + 2 * 3");
    // 1 * (2 + 3)
    Expr *test2 = new Mult(new Num(1), new Add(new Num(2), new Num(3)));
    CHECK((test2 -> to_pretty_string()) == "1 * (2 + 3)");
    // 1 * (2 + 3) * 4
    Expr *test3 = new Mult(new Num(1), new Mult(new Add(new Num(2), new Num(3)), new Num(4)));
    CHECK((test3 -> to_pretty_string()) == "1 * (2 + 3) * 4");
    // (1 + 2) * (3 + 4)
    Expr *test4 = new Mult(new Add(new Num(1), new Num(2)), new Add(new Num(3), new Num(4)));
    CHECK((test4 -> to_pretty_string()) == "(1 + 2) * (3 + 4)");
    // (1 * (2 + 3)) * 4
    Expr *test5 = new Mult(new Mult(new Num(1), new Add(new Num(2), new Num(3))), new Num(4));
    CHECK((test5 -> to_pretty_string()) == "(1 * (2 + 3)) * 4");
}

TEST_CASE("Var Pretty Print"){
    Expr *test1 = new Var("Y");
    CHECK((test1 -> to_pretty_string()) == "Y");
}

TEST_CASE("_let Pretty Print"){
    // test1 =
    //        _let x = 5
    //        _in x + 1
    Expr *test1 = new _let(new Var("x"),
                           new Num(5),
                           new Add(new Var("x"), new Num(1)));
    CHECK((test1 -> to_pretty_string()) == "_let x = 5\n_in x + 1");
    // test2 =
    //        (_let x = 5
    //        _in x) + 1
    Expr *test2 = new Add(new _let(new Var("x"), new Num(5), new Var("x")),
                          new Num(1));
    CHECK((test2 -> to_pretty_string()) == "(_let x = 5\n _in x) + 1");
    // test3 =
    //        5 * (_let x = 5
    //             _in  x) + 1
    Expr *test3 = new Add (new Mult(new Num(5),
                                    new _let(new Var("x"), new Num(5), new Var("x"))),
                           new Num(1));
    CHECK((test3 -> to_pretty_string()) == "5 * (_let x = 5\n"
                                           "     _in x) + 1");
    // test4 =
    //        5 * _let x = 5
    //            _in  x + 1
    Expr *test4 = new Mult(new Num(5),
                           new _let(new Var("x"),
                                    new Num(5),
                                    new Add(new Var("x"), new Num(1))));
    CHECK((test4 -> to_pretty_string()) == "5 * _let x = 5\n"
                                           "    _in x + 1");
    // test5 =
    //        _let x = 5
    //        _in _let x = 3
    //            _in x + 1
    Expr *test5 = new _let(new Var("x"),
                           new Num(5),
                           new _let(new Var("x"), new Num(3),
                                   new Add(new Var("x"), new Num(1))));
    CHECK((test5 -> to_pretty_string()) == "_let x = 5\n"
                                           "_in _let x = 3\n"
                                           "    _in x + 1");
    // test6 =
    //        _let x = _let x = 3
    //                 _in x + 1
    //        _in x + 1
    Expr *test6 = new _let(new Var("x"),
                           new _let(new Var("x"), new Num(3),
                                    new Add(new Var("x"), new Num(1))),
                           new Add(new Var("x"), new Num(1)));
    CHECK((test6 -> to_pretty_string()) == "_let x = _let x = 3\n"
                                           "         _in x + 1\n"
                                           "_in x + 1");
    // test7 =
    //       (5 + (_let x = _let x = 1
    //                      _in x + 2
    //             _in x + 3)) * 4
    Expr *test7 = new Mult(new Add(new Num(5),
                                   new _let(new Var("x"),
                                            new _let(new Var("x"),
                                                     new Num(1),
                                                     new Add(new Var("x"), new Num(2))),
                                            new Add(new Var("x"), new Num(3)))),
                           new Num(4));
    CHECK((test7 -> to_pretty_string()) == "(5 + (_let x = _let x = 1\n"
                                           "               _in x + 2\n"
                                           "      _in x + 3)) * 4");
}



// PARSE TESTS

TEST_CASE("Parse Num"){
    CHECK(parse("3234") -> equals (new Num(3234)));
    CHECK(parse(" 211") -> equals (new Num(211)));
    CHECK(parse("-19 ") -> equals (new Num(-19)));
    CHECK(parse("( -3    )") -> equals (new Num(-3)));
    CHECK_THROWS_WITH(parse("(99"), "missing close paranthesis");
}

TEST_CASE("Parse Add"){
    CHECK(parse("2 + 1") -> equals (new Add(new Num(2), new Num(1))));
    CHECK(parse("-7 + 6") -> equals (new Add(new Num(-7), new Num(6))));
    CHECK(parse("(3 + 2)") -> equals (new Add(new Num(3), new Num(2))));
    CHECK(parse("   5+1") -> equals (new Add(new Num(5), new Num(1))));
    CHECK_THROWS_WITH(parse("(9 +"), "invalid input");
    CHECK_THROWS_WITH(parse("(9 +1"), "missing close paranthesis");
    CHECK_THROWS_WITH(parse("9 +)"), "invalid input");
}

TEST_CASE("Parse Mult"){
    CHECK(parse("6 * 12") -> equals (new Mult(new Num(6), new Num(12))));
    CHECK(parse("-1*2") -> equals (new Mult(new Num(-1), new Num(2))));
    CHECK(parse("(-8)*  4") -> equals (new Mult(new Num(-8), new Num(4))));
    CHECK(parse("(2  * 1)") -> equals (new Mult(new Num(2), new Num(1))));
    CHECK_THROWS_WITH(parse("(2  * 1"), "missing close paranthesis");
    // NEED TO BE FIXED!!!
    // CHECK_THROWS_WITH(parse("2  * 1)"), "invalid input");
}

TEST_CASE("Parse Var"){
    CHECK(parse("cat") -> equals (new Var("cat")));
    CHECK(parse("  dog") -> equals (new Var("dog")));
    CHECK(parse("OWLS") -> equals (new Var("OWLS")));
    // NEED TO BE FIXED!!!
    // CHECK_THROWS_WITH(parse("mo.ngo"), "invalid input");
}

TEST_CASE("Parse _let"){
    CHECK(parse("_let x = 5 _in x+2") -> equals
          (new _let(new Var("x"), new Num(5),
                    new Add(new Var("x"), new Num(2)))));
    CHECK(parse("_let x = (x+2) _in      (x+-3)") -> equals
          (new _let(new Var("x"), new Add(new Var("x"), new Num(2)),
                    new Add(new Var("x"), new Num(-3)))));
    CHECK_THROWS_WITH(parse("_let x = 1    _i"), "invalid input");
}

TEST_CASE("Mixed Parse"){
    CHECK(parse("6 + (2 * -7)") -> equals(new Add(new Num(6),
                                                  new Mult(new Num(2), new Num(-7)))));
    CHECK(parse("(-3)  +  4 * (_let x = 2 _in x+1)") -> equals(
          new Add(new Num(-3), new Mult(new Num(4),
                                        new _let(new Var("x"), new Num(2),
                                                 new Add(new Var("x"), new Num(1)))))));
    CHECK(parse("(1234*((_letx=1_inx+-2)+7))") -> equals(
          new Mult(new Num(1234),
                   new Add(new _let(new Var("x"), new Num(1),
                                    new Add(new Var("x"), new Num(-2))),
                           new Num(7)))));
}
