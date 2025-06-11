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

const unsigned char png_data[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48,
                                  0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x06, 0x00, 0x00,
                                  0x00, 0x1F, 0x15, 0xC4, 0x89, 0x00, 0x00, 0x00, 0x0A, 0x49, 0x44, 0x41, 0x54, 0x78,
                                  0x9C, 0x63, 0x00, 0x01, 0x00, 0x00, 0x05, 0x00, 0x01, 0x0D, 0x0A, 0x2D, 0xB4, 0x00,
                                  0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
const size_t png_data_len = sizeof(png_data);

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
        .column("url", "TEXT")
        .column("method", "TEXT")
        .execute();

    httplib::Server srv;

    srv.Get("/favicon.ico", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(reinterpret_cast<const char*>(png_data), png_data_len, "image/png");
        res.status = 200;
    });

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
            j["url"] = row.getText(3);
            j["method"] = row.getText(4);
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

        auto stmt = db.prepare("INSERT INTO cap (data, meta, url, method) VALUES (?, ?, ?, ?)");
        stmt->bind(1, req.body);
        stmt->bind(2, j.dump());
        stmt->bind(3, req.path);
        stmt->bind(4, req.method);
        stmt->execute();

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
