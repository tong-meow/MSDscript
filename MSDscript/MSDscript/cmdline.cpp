//
//  cmdline.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/17/22.
//
#define CATCH_CONFIG_RUNNER

#include <iostream>
#include <stdexcept>
#include "cmdline.hpp"
#include "catch.hpp"
#include "parse.hpp"
#include "expr.hpp"

void use_arguments(int argc, char **argv){
    // if argc == "./msdscript"
    if(argc == 1){
        return;
    }

    // if argc >= 2
    int index = 1;
    // add a boolean to record if the program is tested or not
    bool tested = false;
    
    while(index <= argc-1){
        
        std::string command = argv[index];
        
        if ( command == "--help") {
            std::cout << "-------------------\n";
            std::cout << "HELP MESSAGE:\n";
            std::cout << "\t--help: display the command line arguments menu.\n";
            std::cout << "\t--test: test this program.\n";
            std::cout << "\t--interp: input an expression and get its value.\n";
            std::cout << "\t--print: input an expression and get a printed result.\n";
            std::cout << "\t--pretty-print: input an expression and get the pretty print result.\n";
            std::cout << "-------------------\n";
            return;
        }
        else if ( command == "--test" ){
            if (!tested) {
                int result = Catch::Session().run(1, argv);
                tested = true;
                if (result == 0){
                    std::cout << "Tests passed.\n";
                }else{
                    std::cerr << result << " test(s) failed.\n";
                    exit(1);
                }
            }else{
                std::cout << "Already tested.\n";
                return;
            }
        }
        else if (command == "--interp"){
            std::cout << "Please enter an expression:\n";
            // parse the std::cin
            Expr *expr = parse_expr(std::cin);
            try {
                // interp the std::cin
                int result = expr -> interp();
                // print out result
                std::cout << "Result after interpation: " << result << "\n";
            } catch (std::runtime_error& e){
                std::cout << e.what() << std::endl;
            }
            return;
        }
        else if (command == "--print"){
            std::cout << "Please enter an expression:\n";
            // parse the std::cin
            Expr *expr = parse_expr(std::cin);
            // print the std::cin
            std::cout << "Result: ";
            expr -> print(std::cout);
            std::cout << "\n";
            return;
        }
        else if (command == "--pretty-print"){
            std::cout << "Please enter an expression:\n";
            // parse the std::cin
            Expr *expr = parse_expr(std::cin);
            // print the std::cin
            std::cout << "Result:\n";
            std::string result = expr -> to_pretty_string();
            std::cout << result << "\n";
            return;
        }
        else{
            std::cerr << "Command not found.\n";
            exit(1);
        }
        index++;
    }
}
