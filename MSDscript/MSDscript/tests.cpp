//
//  tests.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/19/22.
//

#include "expr.hpp"
#include "tests.hpp"

// EQUAL TESTS

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


// INTERP TESTS

TEST_CASE("NumInterp"){
    Expr *test = new Num(42);
    CHECK((test-> interp()) == (new Num(42)) -> interp());
    CHECK((test-> interp()) != (new Num(-42)) -> interp());
    CHECK((test-> interp()) != (new Num(0)) -> interp());
}

TEST_CASE("AddInterp"){
    // test == 1 + 2 + 3 == 6
    Expr *test = new Add(new Add(new Num(1), new Num(2)), new Num(3));
    // test == 6
    CHECK((test-> interp()) == 6);
    // test == 0 + 6
    CHECK((test-> interp()) == (new Add(new Num(0), new Num(6)))-> interp());
    // test != (1 * 7) + 3
    CHECK((test-> interp()) !=
          (new Add(new Mult(new Num(1), new Num(7)), new Num(3)))-> interp());
    // throws exception 1
    CHECK_THROWS_WITH( (new Add(new Variable("Wrong"), new Num(1))) -> interp(),
                      "Error: Expr contains a string element." );
    // throws exception 2
    CHECK_THROWS_WITH( (new Add(new Add(new Num(2), new Variable("Wrong")), new Num(1)))
                      -> interp(), "Error: Expr contains a string element." );
}

TEST_CASE("MultInterp"){
    // test == 1 * 2 * 3 == 6
    Expr *test = new Mult(new Mult(new Num(1), new Num(2)), new Num(3));
    // test == 6
    CHECK((test-> interp()) == 6);
    // test == 0 + 6
    CHECK((test-> interp()) == (new Add(new Num(0), new Num(6)))-> interp());
    // test == 2 * 3
    CHECK((test-> interp()) == (new Mult(new Num(2), new Num(3)))-> interp());
    // test != (1 * 7) + 3
    CHECK((test-> interp()) !=
          (new Add(new Mult(new Num(1), new Num(7)), new Num(3)))-> interp());
    // throws exception 1
    CHECK_THROWS_WITH( (new Mult(new Variable("Wrong"), new Num(1))) -> interp(),
                      "Error: Expr contains a string element." );
    // throws exception 2
    CHECK_THROWS_WITH( (new Mult(new Mult(new Num(2), new Variable("Wrong")), new Num(1)))
                      -> interp(), "Error: Expr contains a string element." );
}

TEST_CASE("VariableInterp"){
    // test = "owls"
    Expr *test = new Variable("owls");
    // throws exception 1
    CHECK_THROWS_WITH(test -> interp(), "Error: Expr contains a string element.");
    // throws exception 2
    CHECK_THROWS_WITH((new Mult(new Add(new Num(1), new Variable("Great Horned Owls")),
                                new Mult(new Num(7), new Variable("Barn Owls"))))
                      -> interp(),
                      "Error: Expr contains a string element.");
}


// HAS_VARIABLE TESTS
TEST_CASE("NumHasVariable"){
    CHECK((new Num(2)) -> has_variable() == false);
}

TEST_CASE("AddHasVariable"){
    Expr *test1 = new Add(new Add(new Num(1), new Num(1)), new Num(1));
    CHECK((test1 -> has_variable()) == false);
    Expr *test2 = new Add(new Add(new Num(1), new Variable("owls")), new Num(1));
    CHECK((test2 -> has_variable()) == true);
}

TEST_CASE("MultHasVariable"){
    Expr *test1 = new Mult(new Add(new Num(1), new Num(1)), new Num(1));
    CHECK((test1 -> has_variable()) == false);
    Expr *test2 = new Mult(new Mult(new Num(1), new Variable("owls")), new Num(1));
    CHECK((test2 -> has_variable()) == true);
}

TEST_CASE("VariableHasVariable"){
    CHECK((new Variable("owls")) -> has_variable() == true);
}


// SUBST TESTS
TEST_CASE("NumSubst"){
    CHECK(((new Num(2))->subst("owls", new Variable("snowy owls"))) ->equals((new Num(2))));
    CHECK(((new Num(2))->subst("owls", new Num(3))) ->equals((new Num(2))));
}

TEST_CASE("AddSubst"){
    // test1 contains no string
    Expr *test1 = new Add(new Num(3), new Mult(new Num(1), new Num(5)));
    CHECK(((test1 -> subst("owls", new Variable("snowy owls"))) ->equals(test1)));
    
    // test2 contains target string
    Expr *test2 = new Add(new Num(3), new Mult(new Variable("owls"), new Num(5)));
    CHECK(((test2 -> subst("owls", new Variable("snowy owls"))) ->equals(
       new Add(new Num(3), new Mult(new Variable("snowy owls"), new Num(5))))));
    
    // test3 contains string but != target
    Expr *test3 = new Add(new Num(3), new Mult(new Variable("barn owls"), new Num(5)));
    CHECK(((test3 -> subst("owls", new Variable("snowy owls"))) -> equals(test3)));
}

TEST_CASE("MultSubst"){
    // test1 contains no string
    Expr *test1 = new Mult(new Num(3), new Add(new Num(1), new Num(5)));
    CHECK(((test1 -> subst("owls", new Variable("snowy owls"))) ->equals(test1)));
    
    // test2 contains target string
    Expr *test2 = new Mult(new Num(3), new Add(new Variable("owls"), new Num(5)));
    CHECK(((test2 -> subst("owls", new Variable("snowy owls"))) ->equals(
       new Mult(new Num(3), new Add(new Variable("snowy owls"), new Num(5))))));
    
    // test3 contains string but != target
    Expr *test3 = new Mult(new Num(3), new Mult(new Variable("barn owls"), new Num(5)));
    CHECK(((test3 -> subst("owls", new Variable("snowy owls"))) -> equals(test3)));
}

TEST_CASE("VariableSubst"){
    CHECK(((new Variable("owls"))->subst("owls", new Variable("snowy owls")))
          ->equals((new Variable("snowy owls"))));
    CHECK(((new Variable("barn owls"))->subst("owls", new Variable("snowy owls")))
          ->equals((new Variable("barn owls"))));
}
