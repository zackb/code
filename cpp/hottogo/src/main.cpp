#include "args.h"
#include "runner/daemon.h"
#include "runner/ui.h"

int main(int argc, const char* argv[]) {

    // parse command line arguments
    Args args(argc, argv);

    if (args.daemon()) {
        std::cout << "Using database at: " << args.dbPath() << "\n";
        std::cout << "Starting server at: http://" << args.host() << ":" << args.port() << "\n";
        Daemon daemon(args);
        daemon.run();
    } else {
        ui::UI ui(args);
        ui.run();
    }

    return EXIT_SUCCESS;
}
