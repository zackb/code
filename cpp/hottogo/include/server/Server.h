#pragma once

#include "db/Database.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <string>

class Server {

public:
    Server(const std::string& dbPath);
    ~Server() = default;

    void listen(const std::string& host, int port) { srv.listen(host.c_str(), port); }
    void stop() {
        srv.stop();
        db.close();
    }

private:
    Database db;
    httplib::Server srv;
};
