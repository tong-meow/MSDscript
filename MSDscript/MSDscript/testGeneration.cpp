//
//  testGeneration.cpp
//  MSDscript
//
//  Created by Tong Shen on 2/14/22.
//

#include <string>
#include <iostream>
#include "testGeneration.hpp"

/*
 GENERATING EXPRESSIONS:
 〈expr〉 = 〈number〉                              ---- 30%
         | ( 〈expr〉 )                            ---- 10%
         | 〈expr〉 + 〈expr〉                      ---- 10%
         | 〈expr〉 * 〈expr〉                      ---- 10%
         | 〈variable〉                            ---- 30%
         | _let 〈variable〉 = 〈expr〉 _in 〈expr 〉---- 10%
 */
std::string random_expr_string() {
    // generate a random number randNum
    srand(clock());
    int randNum = rand();

    // 0 <= randNum < 15, generate a random number
    if ((randNum % 100) < 30) {
        return std::to_string(rand());
    }
    // 15 <= randNum < 30, generate a random ( <expr> )
    else if (((randNum % 100) >= 30) && ((randNum % 100) < 40)){
        return "(" + random_expr_string() + ")";
    }
    // 30 <= randNum < 45, generate a random <expr> + <expr>
    else if (((randNum % 100) >= 40) && ((randNum % 100) < 50)){
        return random_expr_string() + "+" + random_expr_string();
    }
    // 45 <= randNum < 60, generate a random <expr> * <expr>
    else if (((randNum % 100) >= 50) && ((randNum % 100) < 60)){
        return random_expr_string() + "*" + random_expr_string();
    }
    // 60 <= randNum < 75, generate a random <variable>
    else if (((randNum % 100) >= 60) && ((randNum % 100) < 90)){
        return random_var();
    }
    // 75 <= randNum < 100, generate a random
    //                  _let <variable> = <expr> _in <expr>
    else{
        return " _let " + random_var()
               + " = " + random_expr_string()
               + " _in " + random_expr_string();
    }
}



/*
 HELPER FUNCTION FOR std::string random_expr_string()
 use this function to generate a random string with English letters
 */
std::string random_var(){
    // generate a random number as the length of the random string
    srand(clock());
    int varLen = rand() % 20 + 1; // control the length between 1 - 20;
    
    std::string result;
    // get every random letter between 'a' - 'z' or 'A' - 'Z'
    for (int i = 0; i < varLen; i++){
        int capital = rand();
        char c;
        if (capital % 2 == 0){
            c = ( rand() % 26 ) + 'a';
        }else{
            c = ( rand() % 26 ) + 'A';
        }
        result += c;
    }
    
    return result;
}
