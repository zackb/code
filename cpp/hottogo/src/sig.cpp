#include "sig.h"
#include <csignal>

namespace {
    std::function<void(int)> signalHandlerFunc;

    extern "C" void signalDispatcher(int sig) {
        if (signalHandlerFunc)
            signalHandlerFunc(sig);
    }
} // namespace

void setSignalHandler(std::function<void(int)> handler) {
    signalHandlerFunc = std::move(handler);

    std::signal(SIGINT, signalDispatcher);  // Ctrl+C
    std::signal(SIGTERM, signalDispatcher); // kill
    std::signal(SIGHUP, signalDispatcher);  // terminal hangup
}
