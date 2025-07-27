#pragma once

#include "db/database.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <string>

class Server {

public:
    explicit Server(const std::string& dbPath);
    ~Server() = default;

    void listen(const std::string& host, int port) { srv.listen(host.c_str(), port); }
    void stop() {
        srv.stop();
        db.close();
    }

    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;
    Server(const Server&) = delete;

private:
    Database db;
    httplib::Server srv;

    void handleCap(const httplib::Request& req, httplib::Response& res);
};
