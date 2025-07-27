#pragma once

#include <functional>

void setSignalHandler(std::function<void(int)> handler);
