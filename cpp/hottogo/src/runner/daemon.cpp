#include "runner/daemon.h"
#include "sig.h"

Daemon::Daemon(Args& args) : server(args) {}

Daemon::~Daemon() = default;

void Daemon::run() {

    server.init();

    setSignalHandler([this](int) {
        std::cout << "Stopping server...\n";
        server.stop();
    });

    server.listen();
}
