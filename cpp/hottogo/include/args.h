#pragma once

#include <iostream>
#include <string>

struct Args {
public:
    Args(const int argc, const char** argv);

    int port() const { return _port; }
    void port(int port) { _port = port; }

    const std::string& dbPath() const { return _dbPath; }
    void dbPath(const std::string& dbPath) { _dbPath = dbPath; }

    const std::string& host() const { return _host; }
    void host(const std::string& host) { _host = host; }

    bool daemon() const { return _daemon; }

    void help() const {
        std::cout << "Usage: program [options]\n"
                  << "Options:\n"
                  << "  --host, -h <host>       Set the host (default:All interfaces)\n"
                  << "  --port, -p <port>       Set the port (default: 8080)\n"
                  << "  --dbpath, -dp <db_path>    Set the database path (default: cap.db)\n"
                  << "  --daemon, -d            Run in daemon mode (default: false)\n"
                  << "  --help, -?              Show this help message\n";
    }

private:
    std::string _host = "0.0.0.0";  // Default host
    int _port = 8080;               // Default port
    std::string _dbPath = "cap.db"; // Default database path
    bool _daemon = false;           // Default daemon mode
};
