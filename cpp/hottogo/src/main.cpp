#include <unordered_map>
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "cpr/response.h"
#include <cpr/cpr.h>
#include <cstdlib>
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

int main(int argc, const char* argv[]) {

    cpr::Response resp = cpr::Get(cpr::Url{"https://ifconfig.co/json"});
    std::cout << resp.text << std::endl;

    httplib::Client cli("https://ifconfig.co");
    auto resp1 = cli.Get("/json");

    std::cout << resp1->body << std::endl;

    httplib::Server svr;

    svr.Get("/api/v1/info", [](const httplib::Request& req, httplib::Response& res) {
        std::unordered_map<std::string, std::string> map;
        map["remote-addr"] = req.remote_addr;
        map["user-agent"] = req.get_header_value("user-agent");
        nlohmann::json j = map;
        res.set_content(j.dump(), "application/json");
        res.status = 200;
    });

    svr.listen("0.0.0.0", 8080);
    return EXIT_SUCCESS;
}
