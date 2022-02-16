//
//  testDriver.cpp
//  MSDscript
//
//  Created by Tong Shen on 2/14/22.
//

#include <iostream>
#include "testGeneration.hpp"
#include "parse.hpp"
#include "exec.hpp"


/*////////////////////   HELPER FUNCTIONS   //////////////////////////*/

/*
 Used to do the random execution.
 ->     typeOfExec: could be "interp", "print" or "pretty-print"
 ->     testStr: generated randomly by testGeneration.cpp
 */
void exeRandomTest(char **argv, std::string typeOfExec, std::string testStr){
    // exec_program takes 3 parameters
    // ->   1st: int argc
    // ->   2nd: const char * const *argv
    // ->   3rd: std::string input
    
    
    // get the executable file name
    std::string execName;
    if (argv[1] == NULL){
        execName = "msdscript";
    }else{
        execName = argv[1];
    }
    const char *const firstPtr = execName.c_str();
    
    // get the 2nd parameter
    std::string secondArg = "--" + typeOfExec;
    const char *const secondPtr = secondArg.c_str();
    const char *const exec_argv[] = { firstPtr, secondPtr };
    
    try {
        // execute the ./msdscript
        ExecResult execResult = exec_program(2, exec_argv, testStr);
        
        // check if execResult's exit code
        // if it is 1, print error message
        if ((execResult.exit_code) == 1){
            std::cout << typeOfExec << "() failed: " << execResult.err << std::endl;
            exit(1);
        }
        
        // if the execution succeeded, print result message
        std::cout << ">>>> " << typeOfExec << "() result:\n" << execResult.out;
        
    }
    // deal with the runtime_error in exec_program()
    catch (std::runtime_error& e) {
        std::cout << "Error occurs: " << e.what() << std::endl;
        exit(1);
    }
}


/*
 Used to do the comparison execution.
 ->     argv: two executables' names
 ->     typeOfExec: could be "interp", "print" or "pretty-print"
 ->     testStr: generated randomly by testGeneration.cpp
 */
void exeCompareTest(char **argv, std::string typeOfExec, std::string testStr){
    // exec_program takes 3 parameters
    // ->   1st: int argc
    // ->   2nd: const char * const *argv
    // ->   3rd: std::string input
    
    
    // get the type of execusion
    std::string secondArg = "--" + typeOfExec;
    const char *const secondPtr = secondArg.c_str();
    
    // get 2 executables' file names
    std::string execName1 = argv[1];
    const char *const firstPtr1 = execName1.c_str();
    std::string execName2 = argv[2];
    const char *const firstPtr2 = execName2.c_str();
    
    // get the 2nd parameters
    const char * const exec1_argv[] = {firstPtr1, secondPtr};
    const char * const exec2_argv[] = {firstPtr2, secondPtr};
    
    // execute 2 executable
    try {
        ExecResult execResult1= exec_program(2, exec1_argv, testStr);
        ExecResult execResult2 = exec_program(2, exec2_argv, testStr);
        
        // check if any executable gets error
        if (execResult1.exit_code == 1){
            std::cout << typeOfExec << "() in 1st executable failed: "
                << execResult1.err;
            exit(1);
        }
        
        if (execResult2.exit_code == 1){
            std::cout << typeOfExec << "() in 2nd executable failed: "
                << execResult2.err;
            exit(1);
        }
        
        if (execResult1.out != execResult2.out){
            std::cout << ">>> " << typeOfExec << "() got different results:\n";
            std::cout << "random test string: " << testStr << std::endl;
            std::cout << "result of 1st executable: " << execResult1.out;
            std::cout << "result of 2nd executable: " << execResult2.out << std::endl;
        }
    }
    // deal with the runtime_error in exec_program()
    catch (std::runtime_error& e) {
        std::cout << "Error occurs: " << e.what() << std::endl;
        exit(1);
    }
}




/*////////////////////   MAIN FUNCTIONS   //////////////////////////*/


int main(int argc, char **argv){
    
    // check argc's value
    // ->   1 || 2 = generate random test cases with an executable
    // ->   3 = compare the results of 2 msdscript executables
    // e.g. $ ./test_msdscript
    //                --> run msdscript with random test cases
    //      $ ./test_msdscript <exectableName>
    //                --> run the executable with random test cases
    //      $ ./test_msdscript <execName1> <execName2>
    //                --> run the 2 executable and compare results
    
    if (argc == 1 || argc == 2){
        
        std::cout << "------- Random Test -------" << std::endl;
        
        // get a random string
        std::string testStr = random_expr_string();
        
        std::cout << "Test input: " << testStr << std::endl;
        
        // print()
        exeRandomTest(argv, "print", testStr);
        // pretty-print()
        exeRandomTest(argv, "pretty-print", testStr);
        // interp()
        exeRandomTest(argv, "interp", testStr);
        
        std::cout << "------- Test Finished -------" << std::endl;
        
        
    }else if (argc == 3){
        
        std::cout << "------- Compare Test -------" << std::endl;
        
        // loop 100 times, check 100 different random test string
        for (int i = 0; i < 100; i++) {
            // get a random string
            std::string testStr = random_expr_string();
            std::cout << "Test input: " << testStr << std::endl;
            // test print()
            exeCompareTest(argv, "print", testStr);
            // test pretty-print()
            exeCompareTest(argv, "pretty-print", testStr);
            // test interp()
            exeCompareTest(argv, "interp", testStr);
        }
        
        std::cout << "------- Test Finished -------" << std::endl;
        
    }else{
        
        std::cout << "Command not found.\n";
        
    }
    
    return 0;
}
