#pragma once

#include "args.h"
#include "runner/runner.h"
#include "server/server.h"

class Daemon : public Runner {

public:
    Daemon(Args& args);
    ~Daemon() override;

    void run() override;

private:
    Server server;
};
