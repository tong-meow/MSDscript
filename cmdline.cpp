//
//  cmdline.cpp
//  hw2
//
//  Created by Tong Shen on 1/17/22.
//
#define CATCH_CONFIG_RUNNER

#include <iostream>
#include "cmdline.hpp"
#include "catch.hpp"

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
        else{
            std::cerr << "Command not found.\n";
            exit(1);
        }
        index++;
    }
}
