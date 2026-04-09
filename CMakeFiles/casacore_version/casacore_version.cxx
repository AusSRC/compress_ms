
#include <iostream>
#include <casacore/casa/version.h>

int main(int argc, char *argv[]) {
    std::cout << casacore::getVersion();
    return 0;
}

