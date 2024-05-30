// Copyright 2024 Vadim Belan

#include <stdexcept>
#include <thread>  // NOLINT [build/c++11]
#include <chrono>  // NOLINT [build/c++11]
#include "TimedDoor.h"

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor& DoorTimerAdapter::getDoor() const {
    return door;
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout)
, isOpened(false), adapter(new DoorTimerAdapter(*this)) {}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door left open!");
}

DoorTimerAdapter* TimedDoor::getAdapter() {
    return adapter;
}

void Timer::tregister(int timeout, TimerClient* client) {
    this->client = client;
    sleep(timeout);
    client->Timeout();
}

void Timer::sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}
