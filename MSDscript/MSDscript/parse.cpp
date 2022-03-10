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


/*////////////////// PARSE NUM / VAR / LET / IF FACTORS //////////////////*/

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
        number = number * (-1);
    }

    return new NumExpr(number);
}


VarExpr *parse_var(std::istream &in){
    std::string variable;
    
    while(true){
        int c = in.peek();
        // the var name support '_', e.g: class_number, studentName_ is considered valid
        if (isalpha(c) || c == '_'){
            consume(in, c);
            variable += c;
        }
        // if the next character is these below, return the var
        // note: 10 is new line
        else if( (c == ' ') || (c == '+') || (c == '=') || (c == ')') ||
                 (c == '*') || (c == 10) || (c == -1)){
            break;
        }
        // if the next character is other chars, e.g: class.., myCat-,
        // these are considered invalid
        else{
            throw std::runtime_error("invalid input (parse_var())");
        }
    }
    
    return new VarExpr(variable);
}

Expr *parse_bool(std::istream &in){
    
    std::string boolean;
    // "_true" or "_false"
    boolean = getNChars(in, 4);
    
    if (boolean == "true"){
        return new BoolExpr(true);
    }
    else if (boolean == "fals"){
        int nextChar = in.get();
        if (nextChar == 'e'){
            return new BoolExpr(false);
        }
        else{
            throw std::runtime_error("invalid input (parse_bool(), '_false')");
        }
    }
    else{
        throw std::runtime_error("invalid input (parse_bool())");
    }
}

Expr *parse_equal(std::istream &in, std::stack<char> &paren){
    Expr* left;
    Expr* right;
    
    left = parse_expr(in, paren, false);
    skip_whitespace(in);
    int equal = in.peek();
    
    if (equal == '='){
        consume(in, '=');
        right = parse_expr(in, paren, false);
    }
    else {
        throw std::runtime_error("invalid input (parse_equal(), '=')");
    }
    
    return new EqualExpr(left, right);
}


Expr *parse_let(std::istream &in, std::stack<char> &paren){
    // 〈multicand〉 =  _let 〈variable〉 = 〈expr〉 _in 〈expr〉
    VarExpr* var;
    Expr* rhs;
    Expr* body;
    
    // CHECK "_let" : get the first 3 char
    // note: "_" is already consumed
    std::string keywordLet;
    keywordLet = getNChars(in, 3);
    
    // if first 3 chars == _let
    if (keywordLet == "let"){
        skip_whitespace(in);
        // get the var
        // if var is not a varExpr, parse_var() prints error message
        var = parse_var(in);
    }
    // if first 3 chars != _let
    else{
        throw std::runtime_error("invalid input (parse_let(), '_let')");
    }
    
    // CHECK "=" : get the "="
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
        throw std::runtime_error("invalid input (parse_let(), '=')");
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
        throw std::runtime_error("invalid input (parse_let(), '_in')");
    }

    return new LetExpr(var, rhs, body);
}


Expr *parse_if(std::istream &in, std::stack<char> &paren){
    // <multicand> = _if <expr> _then <expr> _else <expr>
    Expr *test_part;
    Expr *then_part;
    Expr *else_part;
    
    // CHECK "_if" : get the first 3 char
    // note: "_" is already consumed
    std::string keywordIf;
    keywordIf = getNChars(in, 2);
    
    // if first 3 chars == _if
    if (keywordIf == "if"){
        skip_whitespace(in);
        // get the expr
        int c = in.peek();
        if (c == '_'){
            consume(in, c);
            test_part = parse_bool(in);
        }else{
            test_part = parse_equal(in, paren);
        }
    }
    // if first 3 chars != _if
    else{
        throw std::runtime_error("invalid input (parse_if(), '_if')");
    }
    
    // CHECK "_then"
    skip_whitespace(in);
    std::string keywordThen;
    keywordThen = getNChars(in, 5);
    
    if (keywordThen == "_then"){
        skip_whitespace(in);
        // get the expr
        then_part = parse_expr(in, paren, false);
    }
    else{
        throw std::runtime_error("invalid input (parse_if(), '_then')");
    }
    
    // CHECK "_else"
    std::string keywordElse;
    keywordElse = getNChars(in, 5);
    
    if (keywordElse == "_else"){
        skip_whitespace(in);
        // get the expr
        else_part = parse_expr(in, paren, false);
    }
    else{
        throw std::runtime_error("invalid input (parse_if(), '_else')");
    }
    
    return new IfExpr(test_part, then_part, else_part);
}



/*/////////////PARSE ADDEND / MULTCAND / EXPR STRUCTURES//////////////*/

/*
 〈expr〉 = 〈addend〉
         | 〈addend〉 + 〈expr〉
 */

Expr *parse_expr(std::istream &in, std::stack<char> &paren, bool firstCheck){
    skip_whitespace(in);
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
                return new AddExpr(e, rhs);
            }
            else{
                throw std::runtime_error("invalid input (parse_expr())");
            }
        }else{
            return new AddExpr(e, rhs);
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
            throw std::runtime_error("invalid input (parse_expr())");
        }
    }
}



/*
 〈addend〉 = 〈multicand〉
           | 〈multicand〉 * 〈addend〉
 */

Expr *parse_addend(std::istream &in, std::stack<char> &paren){
    skip_whitespace(in);
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
        return new MultExpr(e, rhs);
    }
    else {
        return e;
    }
}


/*
 <multicand> = <number>
              | ( <expr> )
              | <variable>
              | _let <variable> = <expr> _in <expr>
              | _if <expr> _then <expr> _else <expr>
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
        VarExpr *var = parse_var(in);
        skip_whitespace(in);
        int check = in.peek();
        if (check == ')' && (paren.empty())){
            throw std::runtime_error("missing open parentheses");
        }
        return var;
    }
    
    //  case 4: <multicand> = _let <variable> = <expr> _in <expr>
    //          <multicand> = _if <expr> _then <expr> _else <expr>
    else if (c == '_'){
        consume(in, '_');
        Expr *res = parseUs(in, paren);
        return res;
    }
    
    //  Invalid input
    else{
        consume(in, c);
        throw std::runtime_error("invalid input (parse_multicand())");
    }

}


Expr *parseUs(std::istream &in, std::stack<char> &paren) {
    skip_whitespace(in);
    int check = in.peek();
    Expr *result;

    if (check == 'l') {
        result = parse_let(in, paren);
    }
    else if (check == 'i') {
        result = parse_if(in, paren);
    }
    else if (check == 'f' || check == 't'){
        result = parse_bool(in);
    }
    else {
        throw std::runtime_error("invalid input (parseUs())");
    }
    
    skip_whitespace(in);
    int c = in.peek();
    if (c == ')' && (paren.empty())){
        throw std::runtime_error("missing open parentheses");
    }
    
    return result;
}
