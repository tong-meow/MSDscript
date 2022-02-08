//
//  parse.hpp
//  MSDscript
//
//  Created by Tong Shen on 2/7/22.
//

#ifndef parse_hpp
#define parse_hpp

#include <stdio.h>
#include "expr.hpp"

Expr *parse(std::string str);
void consume(std::istream &in, int c);
void skip_whitespace(std::istream &in);
Expr *parse_num(std::istream &in);
Expr *parse_expr(std::istream &in);
Expr *parse_addend(std::istream &in);
Expr *parse_multicand(std::istream &in);

#endif /* parse_hpp */
