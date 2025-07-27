#include "server/server.h"
#include "db/statement.h"
#include "db/table_builder.h"
#include "server/pixel.h"
#include <nlohmann/json.hpp>
#include <unordered_map>

Server::Server(const std::string& dbPath) : db(dbPath) {
    db.createTable("cap")
        .ifNotExists()
        .column("id", "INTEGER PRIMARY KEY")
        .column("data", "TEXT")
        .column("meta", "TEXT")
        .column("url", "TEXT")
        .column("method", "TEXT")
        .execute();

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

    srv.Get("/api/v1/cap", [db = &db](const httplib::Request& req, httplib::Response& res) {
        std::vector<nlohmann::json> caps;

        auto stmt = db->prepare("SELECT * FROM cap");
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

    srv.Get("/*", [this](const auto& req, auto& res) { handleCap(req, res); });

    srv.Post("/*", [this](const auto& req, auto& res) { handleCap(req, res); });

    srv.set_pre_routing_handler([this](const auto& req, auto& res) {
        handleCap(req, res);
        return httplib::Server::HandlerResponse::Unhandled;
    });
}

void Server::handleCap(const httplib::Request& req, httplib::Response& res) {
    std::unordered_map<std::string, std::string> map;
    for (auto& [k, v] : req.headers)
        map[k] = v;
    for (auto& [k, v] : req.params)
        map[k] = v;
    nlohmann::json j = map;

    auto stmt = db.prepare("INSERT INTO cap (data, meta, url, method) VALUES (?, ?, ?, ?)");
    stmt->bind(1, req.body);
    stmt->bind(2, j.dump());
    stmt->bind(3, req.path);
    stmt->bind(4, req.method);
    stmt->execute();

    res.status = 404;
}
