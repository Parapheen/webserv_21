#pragma once
#include <string>
#include "EventLoop.hpp"
#include <map>

template <class T>
class Manager {
    private:
        std::map<int, T *>          _elements; // usually would contain fd and eventType / server / client

    protected:
        EventLoop                   _eventLoop; // kevent loop

        void                        addOne(const int &key, T *el);
        const std::map<int, T *>    &getAll(void) const;
        T                           *getOne(const int &key);
        void                        removeOne(const int &key);
    
    public:
        Manager(void);
        Manager(const EventLoop &eventLoop);

        Manager(const Manager &instance);
        Manager &operator=(const Manager &rhs);
        virtual ~Manager(void);
};

template <class T>
Manager<T>::Manager(void) { return ; }

template <class T>
Manager<T>::Manager(const EventLoop &eventLoop) : _eventLoop(eventLoop) { return ; }

template <class T>
Manager<T>::Manager(const Manager &instance) : _elements(instance._elements) { return ; }

template <class T>
Manager<T> &Manager<T>::operator=(const Manager &rhs) {
    if (this != &rhs) {
        for (size_t i = 0; i < rhs._elements.size(); i++)
            this->_elements[i] = rhs._elements[i];
    }
    return *this;
}

template <class T>
void    Manager<T>::addOne(const int &key, T *el) {
    this->_elements.insert(std::pair<int, T *>(key, el));
    return ;
}

template <class T>
const std::map<int, T *>    &Manager<T>::getAll(void) const {
    return this->_elements;
}

template <class T>
T *Manager<T>::getOne(const int &key) {
    try {
        return this->_elements.at(key);
    }
    catch (const std::out_of_range& e) {
        return nullptr;
    }
}

template <class T>
void Manager<T>::removeOne(const int &key) {
    delete this->_elements[key];
    this->_elements.erase(key);
}

template <class T>
Manager<T>::~Manager(void) {
    typename std::map<int, T *>::iterator it = this->_elements.begin();
    while (it != this->_elements.end()) {
        if (it->second)
            delete it->second;
        ++it;
    }
    return;
}