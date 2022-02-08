//
//  parse.cpp
//  MSDscript
//
//  Created by Tong Shen on 2/7/22.
//

#include <iostream>
#include <sstream>
#include "parse.hpp"

Expr *parse(std::string str){
    std::stringstream ss;
    ss << str;
    Expr* e = parse_expr(ss);
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
        if (isalpha(c)){
            consume(in, c);
            variable += c;
        }else{
            break;
        }
    }
    
    return new Var(variable);
}

Expr *parse_let(std::istream &in){
    // 〈multicand〉 =  _let 〈variable〉 = 〈expr〉 _in 〈expr〉
    Var* var;
    Expr* rhs;
    Expr* body;
    
    std::string keywordLet;
    for (int i = 0; i < 4; i++){
        keywordLet += in.peek();
        consume(in, in.peek());
    }
    
    if (keywordLet == "_let"){
        skip_whitespace(in);
        var = parse_var(in);
        skip_whitespace(in);
        int equal = in.peek();
        if (equal == '='){
            consume(in, equal);
            skip_whitespace(in);
            
            rhs = parse_expr(in);
            
            skip_whitespace(in);
            std::string keywordIn;
            for (int i = 0; i < 3; i++){
                keywordIn += in.peek();
                consume(in, in.peek());
            }
            if (keywordIn == "_in"){
                skip_whitespace(in);
                body = parse_expr(in);
            }else{
                throw std::runtime_error("invalid input");
            }
        }else{
            throw std::runtime_error("invalid input");
        }
    }else{
        throw std::runtime_error("invalid input");
    }
    return new _let(var, rhs, body);
}


/*
 〈expr〉 = 〈addend〉
         | 〈addend〉 + 〈expr〉
 */


Expr *parse_expr(std::istream &in){
    Expr *e;
    e = parse_addend(in);
    skip_whitespace(in);
    int c = in.peek();
    if (c == '+') {
        consume(in, '+');
        Expr *rhs = parse_expr(in);
        return new Add(e, rhs);
    }
    else {
        return e;
    }
}


/*
 〈addend〉 = 〈multicand〉
           | 〈multicand〉 * 〈addend〉
 */
Expr *parse_addend(std::istream &in){
    Expr *e;
    e = parse_multicand(in);
    skip_whitespace(in);
    int c = in.peek();
    if (c == '*') {
        consume(in, '*');
        Expr *rhs = parse_addend(in);
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
Expr *parse_multicand(std::istream &in){
    skip_whitespace(in);

    int c = in.peek();
    // 〈multicand〉 = 〈number〉
    if ((c == '-') || isdigit(c)){
        return parse_num(in);
    }
    // 〈multicand〉 =  ( 〈expr〉 )
    else if (c == '(') {
        consume(in, '(');
        Expr *e = parse_expr(in);
        skip_whitespace(in);
        c = in.get();
        if (c != ')'){
            throw std::runtime_error("missing close paranthesis");
        }
        return e;
    }
    // 〈multicand〉 = 〈variable〉
    else if (isalpha(c)){
        return parse_var(in);
    }
    // 〈multicand〉 =  _let 〈variable〉 = 〈expr〉 _in 〈expr〉
    else if (c == '_'){
        return parse_let(in);
    }
    //  Invalid input
    else{
        consume(in, c);
        throw std::runtime_error("invalid input");
    }

}
