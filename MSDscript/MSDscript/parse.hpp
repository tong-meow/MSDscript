//
//  parse.hpp
//  MSDscript
//
//  Created by Tong Shen on 2/7/22.
//

#ifndef parse_hpp
#define parse_hpp

#include <stdio.h>
#include <stack>
#include "expr.hpp"

Expr *parse(std::string str);

void consume(std::istream &in, int c);
void skip_whitespace(std::istream &in);
std::string getNChars(std::istream &in, int numOfChars);

Expr *parse_num(std::istream &in);
VarExpr *parse_var(std::istream &in);
Expr *parse_let(std::istream &in, std::stack<char> &paren);

Expr *parse_expr(std::istream &in, std::stack<char> &paren, bool firstCheck);
Expr *parse_addend(std::istream &in, std::stack<char> &paren);
Expr *parse_multicand(std::istream &in, std::stack<char> &paren);

#endif /* parse_hpp */
