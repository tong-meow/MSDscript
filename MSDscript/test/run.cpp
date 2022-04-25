//
//  run.cpp
//  test
//
//  Created by Tong Shen on 1/24/22.
//

extern "C" {
#include "run.hpp"
};

#define CATCH_CONFIG_RUNNER
#include "../MSDscript/catch.hpp"

bool run_tests() {
    const char *argv[] = { "arith" };
    return (Catch::Session().run(1, argv) == 0);
}
