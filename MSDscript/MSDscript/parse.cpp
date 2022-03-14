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

/*////////////////// HELPER FUNCTIONS //////////////////*/

Expr *parse(std::string str){
    std::stringstream ss;
    ss << str;
    Expr* e = parse_expr(ss);
    skip_whitespace(ss);
    if (!ss.eof()){
        throw std::runtime_error("Invalid input (parse).");
    }
    return e;
}

Expr *parse_stream(std::istream &in){
    Expr* e = parse_expr(in);
    skip_whitespace(in);
    return e;
}


void consume(std::istream &in, int c){
    if (in.peek() == c){
        in.get();
    }
    else{
        throw std::runtime_error("Consume char doesn't match.");
    }
}


void skip_whitespace(std::istream &in){
    while(true){
        int c = in.peek();
        if (!isspace(c)){
            break;
        }
        consume(in, c);
    }
}


std::string get_keyword(std::istream &in){
    std::string keyword;
    
    int c = in.peek();
    
    // ==
    if (c == '='){
        keyword += c;
        consume(in, c);
        c = in.peek();
        keyword += c;
        if (keyword == "=="){
            consume(in, '=');
            return keyword;
        }else{
            throw std::runtime_error("Invalid input ('==' keyword).");
        }
    }
    
    while(true){
        if ( (c != '(') && (c != EOF) &&
            (!isdigit(c)) && (!isspace(c))){
            keyword += c;
            consume(in, c);
            c = in.peek();
        }else{
            break;
        }
    }
    return keyword;
}

// used for parse_var() to check if a char should be include in a var name
bool check_var_validity(int c){
    if (c == EOF || isspace(c)){
        return false;
    }
    if (c == '+' || c == '=' || c == '*' || c == '(' || c == ')'){
        return false;
    }
    return true;
}


/*////////////////// PARSE NUM / VAR FACTORS //////////////////*/

Expr *parse_num(std::istream &in){
    int number = 0;
    bool negative = false;

    if (in.peek() == '-') {
        negative = true;
        consume(in, '-');
        int c = in.peek();
        if (c == EOF || c == ' '){
            throw std::runtime_error("Invalid input (parse_num).");
        }
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


Expr *parse_var(std::istream &in){
    std::string variable;
    int c = in.peek();
    while(check_var_validity(c)){
        // the var name support '_', e.g: class_number, studentName_ is considered valid
        if (isalpha(c) || c == '_'){
            consume(in, c);
            variable += c;
        }
        // if the next character is other chars, e.g: class.., myCat-,
        // these are considered invalid
        else{
            throw std::runtime_error("Invalid input (parse_var).");
        }
        c = in.peek();
    }
    
    return new VarExpr(variable);
}



/*////////////////// PARSE _LET / _IF / _FUN FACTORS //////////////////*/

Expr *parse_let(std::istream &in){
    // _let <variable> = <expr> _in <expr>
    VarExpr* var;
    Expr* rhs;
    Expr* body;
    
    // parse var
    skip_whitespace(in);
    var = dynamic_cast<VarExpr*>(parse_var(in));
    if (var == NULL){
        throw std::runtime_error("Invalid input (parse_let, parse_var).");
    }
    
    // check if there is an "="
    skip_whitespace(in);
    int c = in.peek();
    if (c == '='){
        consume(in, c);
        skip_whitespace(in);
        rhs = parse_expr(in);
    }
    else{
        throw std::runtime_error("Invalid input (parse_let).");
    }
    
    // check if there is a "_in"
    skip_whitespace(in);
    std::string keyword = get_keyword(in);
    if (keyword == "_in"){
        skip_whitespace(in);
        body = parse_expr(in);
    }
    else{
        throw std::runtime_error("Invalid input (parse_let).");
    }
    
    skip_whitespace(in);
    return new LetExpr(var, rhs, body);
}


Expr *parse_if(std::istream &in){
    // _if <expr> _then <expr> _else <expr>
    Expr *test_part;
    Expr *then_part;
    Expr *else_part;
    
    // parse test_part
    skip_whitespace(in);
    test_part = parse_expr(in);
    
    // check if there is a "_then"
    skip_whitespace(in);
    std::string keyword = get_keyword(in);
    if (keyword == "_then"){
        skip_whitespace(in);
        then_part = parse_expr(in);
    }
    else{
        throw std::runtime_error("Invalid input (parse_if).");
    }
    
    // check if there is an "_else"
    skip_whitespace(in);
    keyword = get_keyword(in);
    if (keyword == "_else"){
        skip_whitespace(in);
        else_part = parse_expr(in);
    }
    else{
        throw std::runtime_error("Invalid input (parse_if).");
    }
    
    return new IfExpr(test_part, then_part, else_part);
}


Expr *parse_fun(std::istream &in){
    // fun (var) = <expr>
    VarExpr* formal_arg;
    Expr* body;
    
    // parse var (function name)
    skip_whitespace(in);
    int c = in.peek();
    if (c == '('){
        consume(in, c);
        skip_whitespace(in);
        formal_arg = dynamic_cast<VarExpr*>(parse_var(in));
        if (formal_arg == NULL){
            throw std::runtime_error("Invalid input (parse_fun).");
        }
        skip_whitespace(in);
        c = in.peek();
        if (c == ')'){
            consume(in, c);
        }
        else{
            throw std::runtime_error("Invalid input (parse_fun).");
        }
    }
    else{
        throw std::runtime_error("Invalid input (parse_fun).");
    }
    
    // parse body expr
    skip_whitespace(in);
    body = parse_expr(in);
    
    return new FunExpr(formal_arg -> to_string(), body);
}



/*///////////// PARSE EXPR / COMPARG / ADDEND / MULTCAND STRUCTURES //////////////*/

/*
 〈expr〉     = 〈comparg〉
             | 〈comparg〉 == 〈expr〉
〈comparg〉   = 〈addend〉
             | 〈addend〉 + 〈comparg〉
〈addend〉    = 〈multicand〉
             | 〈multicand〉 * 〈addend〉
〈multicand〉 = 〈inner〉
             | 〈multicand〉 ( 〈expr〉 )
〈inner〉     = 〈number〉 | ( 〈expr〉 ) | 〈variable〉
             | _let 〈variable〉 = 〈expr〉 _in 〈expr〉
             | _true | _false
             | _if 〈expr〉 _then 〈expr〉 _else 〈expr〉
             | _fun ( 〈variable〉 ) 〈expr〉
 */

Expr *parse_expr(std::istream &in){
    skip_whitespace(in);
    // try to parse a comparg
    Expr *comparg = parse_comparg(in);
    skip_whitespace(in);
    
    // check if there is an '='
    // if there is, parse the next <expr>
    int c = in.peek();
    if (c == '=') {
        std::string keyword = get_keyword(in);
        if (keyword == "=="){
            skip_whitespace(in);
            Expr *e = parse_expr(in);
            return new EqualExpr(comparg, e);
        }
        else{
            throw std::runtime_error("Invalid input (parse_expr).");
        }
    }
    return comparg;
}


Expr *parse_comparg(std::istream &in){
    skip_whitespace(in);
    // try to parse an addend
    Expr *addend = parse_addend(in);
    skip_whitespace(in);
    
    // check if there is a '+'
    // if there is, parse the next <comparg>
    int c = in.peek();
    if (c == '+'){
        consume(in, c);
        skip_whitespace(in);
        Expr *comparg = parse_comparg(in);
        return new AddExpr(addend, comparg);
    }
    else {
        return addend;
    }
}


Expr *parse_addend(std::istream &in){
    skip_whitespace(in);
    // try to parse a multicand
    Expr *multicand = parse_multicand(in);
    skip_whitespace(in);
    
    // check if there is a '*'
    // if there is, parse the next <addend>
    int c = in.peek();
    if (c == '*') {
        consume(in, c);
        skip_whitespace(in);
        Expr *addend = parse_addend(in);
        return new MultExpr(multicand, addend);
    }
    else {
        return multicand;
    }
}


Expr *parse_multicand(std::istream &in){
    skip_whitespace(in);
    // try to parse an inner
    Expr* e = parse_inner(in);
    
    Expr* actual_arg;
    skip_whitespace(in);
    
    while (in.peek() == '(') {
        consume(in, '(');
        skip_whitespace(in);
        actual_arg = parse_expr(in);
        skip_whitespace(in);
        consume(in, ')');
        e = new CallExpr(e, actual_arg);
    }
    skip_whitespace(in);
    
    return e;
}


Expr *parse_inner(std::istream &in) {
    skip_whitespace(in);
    int check = in.peek();
    // if is an NumExpr
    if (check == '-' || isdigit(check)){
        return parse_num(in);
    }
    // if is a VarExpr
    else if (isalpha(check)){
        return parse_var(in);
    }
    // if has paranthesis
    else if (check == '('){
        consume(in, '(');
        skip_whitespace(in);
        Expr *e = parse_expr(in);
        skip_whitespace(in);
        // check if a closing paranthesis is missing
        int c = in.peek();
        if (c != ')'){
            throw std::runtime_error("Missing closing paranthesis.");
        }
        else {
            consume(in, c);
        }
        return e;
    }
    // if is a keyword (_let / _if / _fun / _true / _false)
    else if (check == '_'){
        std::string keyword = get_keyword(in);
        if (keyword == "_let"){
            return parse_let(in);
        }
        else if (keyword == "_if"){
            return parse_if(in);
        }
        else if (keyword == "_fun"){
            return parse_fun(in);
        }
        else if (keyword == "_true"){
            return new BoolExpr(true);
        }
        else if (keyword == "_false"){
            return new BoolExpr(false);
        }
        else{
            throw std::runtime_error("Invalid input (parse_inner, keyword error)");
        }
    }
    // others (handle error)
    else {
        throw std::runtime_error("Invalid input (parse_inner).");
    }
}
