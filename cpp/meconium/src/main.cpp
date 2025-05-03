#include <iostream>
#include "Meconium.h"

int main(int argc, char* argv[]) {

    auto meconium = Meconium();
    if (!meconium.init()) {
        return EXIT_FAILURE;
    }

    meconium.run();

    meconium.shutdown();

    return EXIT_SUCCESS;
}

