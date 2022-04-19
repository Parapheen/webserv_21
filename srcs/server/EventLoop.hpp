#pragma once
#include <sys/event.h>
#include <unistd.h>

class EventLoop {
    private:
        int                    _loop;
        int                    _evListSize;

    public:
        EventLoop(void);

        EventLoop(const EventLoop &instance);
        EventLoop   &operator=(const EventLoop &rhs);

        ~EventLoop(void);

        const int           &getEventLoop(void) const;
        const int           &getEventListSize(void) const;
        void                setReadEvent(void);
};