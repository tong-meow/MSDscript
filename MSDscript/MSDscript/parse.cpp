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
        if (isalpha(c) || c == '_'){
            consume(in, c);
            variable += c;
        }else if( (c == ' ') || (c == '+') || (c == '=') || (c == ')')
                 || (c == 10) || (c == -1)){
            break;
        }else{
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
            
            rhs = parse_expr(in, paren, false);
            
            skip_whitespace(in);
            std::string keywordIn;
            for (int i = 0; i < 3; i++){
                keywordIn += in.peek();
                consume(in, in.peek());
            }
            if (keywordIn == "_in"){
                skip_whitespace(in);
                body = parse_expr(in, paren, false);
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

Expr *parse_expr(std::istream &in, std::stack<char> &paren, bool firstCheck){
    Expr *e;
    e = parse_addend(in, paren);
    skip_whitespace(in);
    int c = in.peek();
    if (c == '+') {
        consume(in, '+');
        Expr *rhs = parse_expr(in, paren, false);
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
        }
    }
    
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
    Expr *e;
    e = parse_multicand(in, paren);
    skip_whitespace(in);
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
    // 〈multicand〉 = 〈number〉
    if ((c == '-') || isdigit(c)){
        Expr *num = parse_num(in);
        skip_whitespace(in);
        int check = in.peek();
        if (check == ')' && (paren.empty())){
            throw std::runtime_error("missing open parentheses");
        }
        return num;
    }
    // 〈multicand〉 =  ( 〈expr〉 )
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
    // 〈multicand〉 = 〈variable〉
    else if (isalpha(c)){
        Var *var = parse_var(in);
        skip_whitespace(in);
        int check = in.peek();
        if (check == ')' && (paren.empty())){
            throw std::runtime_error("missing open parentheses");
        }
        return var;
    }
    // 〈multicand〉 =  _let 〈variable〉 = 〈expr〉 _in 〈expr〉
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
