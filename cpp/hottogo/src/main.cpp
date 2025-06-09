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

    Database db("cap.db");

    db.createTable("cap")
        .ifNotExists()
        .column("id", "INTEGER PRIMARY KEY")
        .column("data", "TEXT")
        .column("meta", "TEXT")
        .execute();

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

    srv.Get("/api/v1/cap", [&](const httplib::Request& req, httplib::Response& res) {
        std::vector<nlohmann::json> caps;

        auto stmt = db.prepare("SELECT * FROM cap");
        for (auto row : *stmt) {
            nlohmann::json j;
            j["id"] = row.getInt(0);
            j["data"] = row.getText(1);
            j["meta"] = row.getText(2);
            caps.push_back(j);
        }
        res.set_content(nlohmann::json(caps).dump(), "application/json");
        res.status = 200;
    });

    auto cap = [&](const httplib::Request& req, httplib::Response& res) {
        std::unordered_map<std::string, std::string> map;
        for (auto& [k, v] : req.headers) {
            map[k] = v;
        }

        for (auto& [k, v] : req.params) {
            map[k] = v;
        }

        nlohmann::json j = map;

        // req.body;

        auto stmt = db.prepare("INSERT INTO cap (data, meta) VALUES (?, ?)");
        stmt->bind(1, req.body);
        stmt->bind(2, j.dump());
        stmt->step();

        res.status = 404;
    };
    srv.Get("/*", cap);
    srv.Post("/*", cap);
    srv.set_pre_routing_handler([&](const auto& req, auto& res) {
        cap(req, res);
        return httplib::Server::HandlerResponse::Unhandled;
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
