/**
 *   EtherTerm - UNIT / INTEGRATION TESTS
 */

#ifdef _WIN32
#include <UnitTest++.h>
#else
#include <unittest++/UnitTest++.h>
#endif


// C Standard
#include <cstdio>
#include <cstring>
// C++ Standard
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "common_io_test.hpp"


// run all unit tests
int main(int argc, char **argv)
{    
    int result = UnitTest::RunAllTests();
        
    return result;
}
