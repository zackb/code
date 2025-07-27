#pragma once

#include "args.h"
#include "db/database.h"
#include <optional>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <string>

class Server {

public:
    explicit Server(Args& args);
    ~Server() = default;

    void init();
    void listen() { srv.listen(args.host().c_str(), args.port()); }
    void start() {
        serverThread.emplace([this]() { listen(); });
    }
    void stop() {
        srv.stop();
        if (serverThread && serverThread->joinable()) {
            serverThread->join();
        }
        db.close();
    }

    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;
    Server(const Server&) = delete;

private:
    Database db;
    httplib::Server srv;
    Args& args;
    std::optional<std::thread> serverThread;

    void handleCap(const httplib::Request& req, httplib::Response& res);
};
