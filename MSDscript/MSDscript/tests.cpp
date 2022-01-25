//
//  tests.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/19/22.
//

#include <iostream>
#include <sstream>
#include "expr.hpp"
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
    CHECK((new Num(2))-> equals(new Mult(new Num(3), new Num(6))) == false);
}

TEST_CASE("Mult Equals"){
    Expr *test = new Mult(new Num(3), new Num(6));
    CHECK(test-> equals(new Mult(new Num(3), new Num(6))) == true);
    CHECK(test-> equals(new Mult(new Num(6), new Num(3))) == false);
    CHECK(test-> equals(new Mult(new Num(1), new Num(2))) == false);
    CHECK((new Num(2))-> equals(new Add(new Num(2), new Num(5))) == false);
}

TEST_CASE("Variable Equals"){
    Expr *test = new Variable("Happy2022");
    CHECK(test-> equals(new Variable("Happy2022")) == true);
    CHECK(test-> equals(new Variable("PlayStation")) == false);
    CHECK(test-> equals(new Variable("XBox")) == false);
    CHECK(test-> equals(new Mult(new Num(1), new Num(2))) == false);
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
    CHECK_THROWS_WITH( (new Add(new Variable("Wrong"), new Num(1))) -> interp(),
                      "Error: Expr contains a string element." );
}

TEST_CASE("Mult Interp"){
    // test == 1 * 2 * 3 == 6
    Expr *test = new Mult(new Mult(new Num(1), new Num(2)), new Num(3));
    // test == 6
    CHECK((test-> interp()) == 6);
    // throws exception
    CHECK_THROWS_WITH( (new Mult(new Variable("Wrong"), new Num(1))) -> interp(),
                      "Error: Expr contains a string element." );
}

TEST_CASE("Variable Interp"){
    // test = "owls"
    Expr *test = new Variable("owls");
    // throws exception
    CHECK_THROWS_WITH(test -> interp(), "Error: Expr contains a string element.");
}



// HAS_VARIABLE TESTS

TEST_CASE("Num Has Variable"){
    CHECK((new Num(2)) -> has_variable() == false);
}

TEST_CASE("Add Has Variable"){
    Expr *test1 = new Add(new Add(new Num(1), new Num(1)), new Num(1));
    CHECK((test1 -> has_variable()) == false);
    Expr *test2 = new Add(new Add(new Num(1), new Variable("owls")), new Num(1));
    CHECK((test2 -> has_variable()) == true);
}

TEST_CASE("Mult Has Variable"){
    Expr *test1 = new Mult(new Add(new Num(1), new Num(1)), new Num(1));
    CHECK((test1 -> has_variable()) == false);
    Expr *test2 = new Mult(new Mult(new Num(1), new Variable("owls")), new Num(1));
    CHECK((test2 -> has_variable()) == true);
}

TEST_CASE("Variable Has Variable"){
    CHECK((new Variable("owls")) -> has_variable() == true);
}



// SUBST TESTS

TEST_CASE("Num Subst"){
    CHECK(((new Num(2))->subst("owls", new Variable("snowy owls"))) ->equals((new Num(2))));
    CHECK(((new Num(2))->subst("owls", new Num(3))) ->equals((new Num(2))));
}

TEST_CASE("Add Subst"){
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

TEST_CASE("Mult Subst"){
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

TEST_CASE("Variable Subst"){
    CHECK(((new Variable("owls"))->subst("owls", new Variable("snowy owls")))
          ->equals((new Variable("snowy owls"))));
    CHECK(((new Variable("barn owls"))->subst("owls", new Variable("snowy owls")))
          ->equals((new Variable("barn owls"))));
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
    Expr *test1 = new Variable("X");
    CHECK((test1 -> to_string()) == "X");
}



// PRETTY_PRINT TESTS

TEST_CASE("Num Pretty Print"){
    Expr *test1 = new Num(10);
    CHECK((test1 -> to_string()) == "10");
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

TEST_CASE("Variable Pretty Print"){
    Expr *test1 = new Variable("Y");
    CHECK((test1 -> to_string()) == "Y");
}
