#include "Signal.h"
#include "db/Database.h"
#include "db/Statement.h"
#include "db/TableBuilder.h"
#include <unordered_map>
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <cstdlib>
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

int main(int argc, const char* argv[]) {

    // httplib::Client cli("https://ifconfig.co");
    // auto resp1 = cli.Get("/json");

    // std::cout << resp1->body << std::endl;

    Database db("test.db");
    db.createTable("test").ifNotExists().column("id", "INTEGER PRIMARY KEY").column("name", "TEXT").execute();
    auto stmt = db.prepare("INSERT INTO test (name) VALUES (?)");
    stmt->bind(1, "test1");

    httplib::Server srv;

    srv.Get("/api/v1/req", [](const httplib::Request& req, httplib::Response& res) {
        std::unordered_map<std::string, std::string> map;
        map["remote-addr"] = req.remote_addr;
        map["user-agent"] = req.get_header_value("user-agent");
        for (auto& [k, v] : req.headers) {
            map[k] = v;
        }

        for (auto& [k, v] : req.params) {
            map[k] = v;
        }

        nlohmann::json j = map;
        res.set_content(j.dump(), "application/json");
        res.status = 200;
    });

    srv.Get("/api/v1/resp", [](const httplib::Request& req, httplib::Response& res) {
        res.status = req.get_param_value("status").empty() ? 200 : std::stoi(req.get_param_value("status"));
    });

    srv.Get("/api/v1/track", [&](const httplib::Request& req, httplib::Response& res) {
        std::vector<nlohmann::json> tracks;

        auto stmt = db.prepare("SELECT * FROM test");
        for (auto row : *stmt) {
            nlohmann::json j;
            j["id"] = row.getInt(0);
            j["data"] = row.getText(1);
            tracks.push_back(j);
        }
        res.set_content(nlohmann::json(tracks).dump(), "application/json");
        res.status = 200;
    });

    setSignalHandler([&](int) {
        std::cout << "Stopping HTTP server..." << std::endl;
        srv.stop();

        std::cout << "Closing database connection..." << std::endl;
        db.close();
    });

    std::cout << "Starting HTTP server on port 8080..." << std::endl;

    srv.listen("0.0.0.0", 8080);

    return EXIT_SUCCESS;
}
