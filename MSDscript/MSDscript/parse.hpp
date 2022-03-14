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
Expr *parse_stream(std::istream &in);

void consume(std::istream &in, int c);
void skip_whitespace(std::istream &in);
bool check_var_validity(int c);
std::string get_keyword(std::istream &in);

Expr *parse_num(std::istream &in);
Expr *parse_var(std::istream &in);
Expr *parse_comparg(std::istream &in);

Expr *parse_let(std::istream &in);
Expr *parse_if(std::istream &in);
Expr *parse_fun(std::istream &in);

Expr *parse_expr(std::istream &in);
Expr *parse_addend(std::istream &in);
Expr *parse_multicand(std::istream &in);
Expr *parse_inner(std::istream &in);

Expr *parse_usage(std::istream &in);

#endif /* parse_hpp */
