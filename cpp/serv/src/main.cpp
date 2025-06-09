#include "cpr/response.h"
#include <cpr/cpr.h>
#include <cstdlib>
#include <iostream>

int main(int argc, const char* argv[]) {

    cpr::Response resp = cpr::Get(cpr::Url{"https://ifconfig.co/json"});
    std::cout << resp.text << std::endl;

    return EXIT_SUCCESS;
}
