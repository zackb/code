#pragma once
#include <functional>

using ActionFn = std::function<void()>;

struct DelayedAction {
    int remainingMs;
    std::function<void()> action;

    DelayedAction(const int remainingMs, const ActionFn& action) : remainingMs(remainingMs), action(action) {}
};
