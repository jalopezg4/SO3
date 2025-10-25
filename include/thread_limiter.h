#pragma once
#include <cstddef>


class ThreadLimiter {
public:
    explicit ThreadLimiter(size_t max_concurrent);
    void enter();
    void leave();
private:
    struct Impl;
    Impl *pimpl;
};