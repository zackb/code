#include "args.h"
#include <iostream>

Args::Args(const int argc, const char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--host" || arg == "-h") {
            if (i + 1 < argc) {
                _host = argv[++i];
            }
        } else if (arg == "--port" || arg == "-p") {
            if (i + 1 < argc) {
                _port = std::stoi(argv[++i]);
            }
        } else if (arg == "--dbpath" || arg == "-dp") {
            if (i + 1 < argc) {
                _dbPath = argv[++i];
            }
        } else if (arg == "--daemon" || arg == "-d") {
            _daemon = true;
        } else {
            if (arg == "--help" || arg == "-?") {
                help();
                exit(0);
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                help();
                exit(1);
            }
        }
    }
}
