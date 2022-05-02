#include "EventLoop.hpp"

EventLoop::EventLoop(void) : _loop(kqueue()), _evListSize(10) { return ;}

EventLoop::EventLoop(const EventLoop &instance) {
    *this = instance;
    return ;
}

EventLoop &EventLoop::operator=(const EventLoop &rhs) {
    if (this->getEventLoop() != rhs.getEventLoop())
        this->_loop = rhs._loop;
    return *this;
}

const int   &EventLoop::getEventLoop(void) const {
    return this->_loop;
}

const int   &EventLoop::getEventListSize(void) const {
    return this->_evListSize;
}

EventLoop::~EventLoop(void) {
    close(this->_loop);
    return ;
}