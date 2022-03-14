//
//  cmdline.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/17/22.
//
#define CATCH_CONFIG_RUNNER

#include <iostream>
#include <stdexcept>
#include <stack>
#include "cmdline.hpp"
#include "catch.hpp"
#include "parse.hpp"
#include "expr.hpp"
#include "val.hpp"

void use_arguments(int argc, char **argv){
    
    // if argc == "./msdscript"
    if(argc == 1){
        return;
    }

    // if argc >= 2
    // supported:
    //  - ./msdscript --help
    //  - ./msdscript --test
    //  - ./msdscript --interp
    //  - ./msdscript --print
    //  - ./msdscript --pretty-print

    // add a boolean to record if the program is tested or not
    bool tested = false;
    
    for(int index = 1; index < argc; index++){
        
        std::string command = argv[index];
        
        // deal with help manu in terminal
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
        
        // deal with test in terminal
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
        
        // deal with interp in terminal
        else if (command == "--interp"){
            while(true){
                try {
                    // parse the std::cin
                    Expr *expr = parse_stream(std::cin);
                    // interp the std::cin
                    std::string result = (expr -> interp()) -> to_string();
                    // print out result
                    std::cout << result << std::endl;
                } catch (std::runtime_error& e){
                    std::cout << "Error: " << e.what() << std::endl;
                    exit(1);
                }
            }
            return;
        }
        
        // deal with print in terminal
        else if (command == "--print"){
            while(true){
                try{
                    // parse the std::cin
                    Expr *expr = parse_expr(std::cin);
                    // print the std::cin
                    expr -> print(std::cout);
                    std::cout << "\n";
                } catch (std::runtime_error& e){
                    std::cout << "Error: " << e.what() << std::endl;
                    exit(1);
                }
                if (strcmp(argv[index+1], "fin")){
                    return;
                }
            }
        }
        
        // deal with pretty print in terminal
        else if (command == "--pretty-print"){
            while(true){
                try{
                    // parse the std::cin
                    Expr *expr = parse_expr(std::cin);
                    // print the std::cin
                    std::string result = expr -> to_pretty_string();
                    std::cout << result << "\n";
                } catch (std::runtime_error& e){
                    std::cout << "Error: " << e.what() << std::endl;
                    exit(1);
                }
                if (strcmp(argv[index+1], "fin")){
                    return;
                }
            }
        }
        
        else{
            std::cerr << "Command not found.\n";
            exit(1);
        }
    }
}
