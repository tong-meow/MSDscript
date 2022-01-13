#include <iostream>
#include "cmdline.h"

void use_arguments(int argc, char **argv){
    if(argc > 2){
        std::cerr << "Command not found.\n";
        exit(1);
    }

    if(argc == 2){
        std::string command = argv[1];
        if (command == "--help") {
            std::cout << "-------------------\n";
            std::cout << "HELP MESSAGE:\n";
            std::cout << "\t--help: display the command line arguments menu.\n";
            std::cout << "\t--test: test this program.\n";
            std::cout << "-------------------\n";
        }else if (command == "--test") {
            std::cout << "Tests passed.\n";
        }else{
            std::cerr << "Command not found.\n";
            exit(1);
        }
    }
}