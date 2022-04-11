//
//  main.cpp
//  MSDscript
//
//  Created by Tong Shen on 1/17/22.
//
#define CATCH_CONFIG_RUNNER

#include <iostream>
#include "catch.hpp"
#include "parse.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "step.hpp"


int main(int argc, char **argv){

    // if argc == "./msdscript"
    if(argc == 1){
        return 0;
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
    // add a boolean to record if the step mode is activated
    bool step_mode = false;
    
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
            return 0;
        }
        
        // if it is step mode
        else if ( command == "--step" ){
            step_mode = true;
            continue;
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
                    return 1;
                }
            }else{
                std::cout << "Already tested.\n";
                return 0;
            }
        }
        
        // deal with interp in terminal
        else if (command == "--interp"){
            try {
                PTR(Expr) expr = parse_stream(std::cin);
                if (!step_mode){
                    PTR(Val) result = expr -> interp(NEW(EmptyEnv)());
                    std::cout << result -> to_string();
                }
                else {
                    PTR(Val) result = Step::interp_by_steps(expr);
                    std::cout << result -> to_string();
                }
                std::cout << " \n";
            } catch (std::runtime_error& e){
                std::cout << "Error: " << e.what() << std::endl;
                return 1;
            }
            return 0;
        }
        
        // deal with print in terminal
        else if (command == "--print"){
            try {
                PTR(Expr) expr = parse_stream(std::cin);
                expr -> print(std::cout);
                std::cout << std::endl;
            } catch (std::runtime_error& e){
                std::cout << "Error: " << e.what() << std::endl;
                return 1;
            }
            return 0;
        }
        
        // deal with pretty print in terminal
        else if (command == "--pretty-print"){
            try {
                PTR(Expr) expr = parse_stream(std::cin);
                expr -> pretty_print(std::cout);
                std::cout << std::endl;
            } catch (std::runtime_error& e){
                std::cout << "Error: " << e.what() << std::endl;
                return 1;
            }
            return 0;
        }
        
        else{
            std::cerr << "Command not found.\n";
            return 1;
        }
    }
    return 0;

}
