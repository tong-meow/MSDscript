//
//  parse.cpp
//  MSDscript
//
//  Created by Tong Shen on 2/7/22.
//

#include <iostream>
#include <sstream>
#include <stack>
#include "parse.hpp"

/*//////////////////HELPER FUNCTIONS//////////////////*/

Expr *parse(std::string str){
    std::stringstream ss;
    ss << str;
    std::stack<char> paren;
    Expr* e = parse_expr(ss, paren, true);
    return e;
}


void consume(std::istream &in, int c){
    if (in.peek() == c){
        in.get();
    }
}


void skip_whitespace(std::istream &in){
    while (true){
        int c = in.peek();
        if (c != ' '){
            break;
        }else{
            consume(in, c);
        }
    }
}

std::string getNChars(std::istream &in, int numOfChars){
    std::string result;
    for (int i = 0; i < numOfChars; i++){
        result += in.peek();
        consume(in, in.peek());
    }
    return result;
}


/*//////////////////PARSE NUM / VAR / LET FACTORS//////////////////*/

Expr *parse_num(std::istream &in){
    int number = 0;
    bool negative = false;

    if (in.peek() == '-') {
        negative = true;
        consume(in, '-');
    }

    while(true){
        int c = in.peek();
        if (isdigit(c)) {
            consume(in, c);
            number = number * 10 + (c - '0');
        }else{
            break;
        }
    }

    if (negative) {
        number = -number;
    }

    return new Num(number);
}


Var *parse_var(std::istream &in){
    std::string variable;
    while(true){
        int c = in.peek();
        // the var name support '_', e.g: class_number, studentName_ is considered valid
        if (isalpha(c) || c == '_'){
            consume(in, c);
            variable += c;
        }
        // if the next character is these below, return the var
        else if( (c == ' ') || (c == '+') || (c == '=') || (c == ')')
                 || (c == 10) || (c == -1)){
            break;
        }
        // if the next character is other chars, e.g: class.., myCat-,
        // these are considered invalid
        else{
            throw std::runtime_error("invalid input");
        }
    }
    
    return new Var(variable);
}


Expr *parse_let(std::istream &in, std::stack<char> &paren){
    // 〈multicand〉 =  _let 〈variable〉 = 〈expr〉 _in 〈expr〉
    Var* var;
    Expr* rhs;
    Expr* body;
    
    // CHECK "_let" : get the first 4 char
    std::string keywordLet;
    keywordLet = getNChars(in, 4);
    
    // if first 4 chars == _let
    if (keywordLet == "_let"){
        skip_whitespace(in);
        // get the var
        var = parse_var(in);
    }
    // if first 4 chars != _let
    else{
        throw std::runtime_error("invalid input");
    }
    
    // CHECK "= : get the "="
    skip_whitespace(in);
    int equal = in.peek();
    // if the next char is "="
    if (equal == '='){
        consume(in, equal);
        skip_whitespace(in);
        // get the rhs
        rhs = parse_expr(in, paren, false);
    }
    // if "=" is not found
    else{
        throw std::runtime_error("invalid input");
    }
    
    
    // CHECK "_in" : get the next 3 chars, they should be _in
    skip_whitespace(in);
    std::string keywordIn;
    keywordIn = getNChars(in, 3);
    // if the next 3 chars are _in
    if (keywordIn == "_in"){
        skip_whitespace(in);
        // get the body
        body = parse_expr(in, paren, false);
    }
    // if the next 3 chars are not _in
    else{
        throw std::runtime_error("invalid input");
    }

    return new _let(var, rhs, body);
}



/*/////////////PARSE ADDEND / MULTCAND / EXPR STRUCTURES//////////////*/

/*
 〈expr〉 = 〈addend〉
         | 〈addend〉 + 〈expr〉
 */

Expr *parse_expr(std::istream &in, std::stack<char> &paren, bool firstCheck){
    // parse the first <addend>
    Expr *e;
    e = parse_addend(in, paren);
    skip_whitespace(in);
    
    // check if there is '+'
    // if there is, parse the next <expr>
    int c = in.peek();
    if (c == '+') {
        consume(in, '+');
        Expr *rhs = parse_expr(in, paren, false);
        // firstCheck: this is a boolean to record if this is the outermost layer.
        // this is used to check if there is invalid chars after a valid expression.
        // for example:
        //      - we consider (3 + 2) as a valid expression
        //      - we consider (3 + 2).. as an invalid expression
        //      - we consider (3 + 2)))) as an invalid expression
        // therefore, we only check once at the outermost layer (because inner layers
        // of function calling, valid chars is checked).
        if (firstCheck) {
            skip_whitespace(in);
            int c = in.peek();
            if (c == ')'){
                throw std::runtime_error("missing open parentheses");
            }
            if (c == 10 || c == -1){
                return new Add(e, rhs);
            }
            else{
                throw std::runtime_error("invalid input");
            }
        }else{
            return new Add(e, rhs);
        }
    }
    // same as above, but this is the situation when there is no '+'.
    // e.g.:  className..., 3]]!@, etc.
    if (!firstCheck) {
        return e;
    }else{
        skip_whitespace(in);
        int c = in.peek();
        if (c == ')'){
            throw std::runtime_error("missing open parentheses");
        }
        if (c == 10 || c == -1){
            return e;
        }
        else{
            throw std::runtime_error("invalid input");
        }
    }
}



/*
 〈addend〉 = 〈multicand〉
           | 〈multicand〉 * 〈addend〉
 */

Expr *parse_addend(std::istream &in, std::stack<char> &paren){
    // parse the first <multicand>
    Expr *e;
    e = parse_multicand(in, paren);
    skip_whitespace(in);
    
    // check if there is a *
    // if there is, continue parse the sencond part of <addend>
    int c = in.peek();
    if (c == '*') {
        consume(in, '*');
        Expr *rhs = parse_addend(in, paren);
        return new Mult(e, rhs);
    }
    else {
        return e;
    }
}


/*
 〈multicand〉 = 〈number〉
              | ( 〈expr〉 )
              | 〈variable〉
              | _let 〈variable〉 = 〈expr〉 _in 〈expr〉
 */

Expr *parse_multicand(std::istream &in, std::stack<char> &paren){
    skip_whitespace(in);
    int c = in.peek();
    
    //  case 1: <multicand> = <number>
    if ((c == '-') || isdigit(c)){
        Expr *num = parse_num(in);
        skip_whitespace(in);
        int check = in.peek();
        if (check == ')' && (paren.empty())){
            throw std::runtime_error("missing open parentheses");
        }
        return num;
    }
    
    //  case 2: <multicand> = ( <expr> )
    else if (c == '(') {
        paren.push('(');
        consume(in, '(');
        Expr *e = parse_expr(in, paren, false);
        skip_whitespace(in);
        c = in.get();
        if (c != ')'){
            throw std::runtime_error("missing close parentheses");
        }else{
            char previousInParen = paren.top();
            if (previousInParen != '('){
                throw std::runtime_error("missing open parentheses");
            }else{
                paren.pop();
            }
        }
        return e;
    }
    
    //  case 3: <multicand> = <variable>
    else if (isalpha(c)){
        Var *var = parse_var(in);
        skip_whitespace(in);
        int check = in.peek();
        if (check == ')' && (paren.empty())){
            throw std::runtime_error("missing open parentheses");
        }
        return var;
    }
    
    //  case 4: <multicand> = _let <variable> = <expr> _in <expr>
    else if (c == '_'){
        Expr *let = parse_let(in, paren);
        skip_whitespace(in);
        int check = in.peek();
        if (check == ')' && (paren.empty())){
            throw std::runtime_error("missing open parentheses");
        }
        return let;
    }
    
    //  Invalid input
    else{
        consume(in, c);
        throw std::runtime_error("invalid input");
    }

}
