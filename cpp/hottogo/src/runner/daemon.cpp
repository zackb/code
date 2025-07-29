#include "runner/daemon.h"
#include "sig.h"

Daemon::Daemon(Args& args) : server(args) {}

void Daemon::run() {

    server.init();

    setSignalHandler([this](int) {
        std::cout << "Stopping server...\n";
        server.stop();
    });

    server.listen();
}
