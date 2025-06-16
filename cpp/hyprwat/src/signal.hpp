#pragma once

#include <functional>

void signalHandler(std::function<void(int)> handler);
