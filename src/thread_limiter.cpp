#include "thread_limiter.h"
#include <mutex>
#include <condition_variable>


struct ThreadLimiter::Impl {
    size_t max_concurrent;
    size_t active = 0;
    std::mutex m;
    std::condition_variable cv;
    Impl(size_t mnc): max_concurrent(mnc) {}
};


ThreadLimiter::ThreadLimiter(size_t max_concurrent) : pimpl(new Impl(max_concurrent)) {}


void ThreadLimiter::enter() {
    std::unique_lock<std::mutex> lk(pimpl->m);
    pimpl->cv.wait(lk, [&]{ return pimpl->active < pimpl->max_concurrent; });
    ++pimpl->active;
}


void ThreadLimiter::leave() {
    std::unique_lock<std::mutex> lk(pimpl->m);
    if (pimpl->active > 0) --pimpl->active;
    pimpl->cv.notify_one();
}