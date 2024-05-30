// Copyright 2024 Vadim Belan

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>  // NOLINT [build/c++11]
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
    MOCK_METHOD(void, throwState, (), ());
};

class TimedDoorTest : public testing::Test {
 protected:
    TimedDoor* timedDoor;
    Timer* timer;

    void SetUp() override {
        timedDoor = new TimedDoor(2);
        timer = new Timer();
    }

    void TearDown() override {
        delete timedDoor;
        delete timer;
    }
};

TEST_F(TimedDoorTest, DoorInitiallyClosed) {
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockDoor) {
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockDoor) {
    timedDoor->unlock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsExceptionIfDoorOpened) {
    timedDoor->unlock();
    EXPECT_THROW({
        timer->tregister(timedDoor->getTimeOut(), timedDoor->getAdapter());
    }, std::runtime_error);
}

TEST_F(TimedDoorTest, TimeoutDoesNotThrowIfDoorClosed) {
    EXPECT_NO_THROW({
        timer->tregister(timedDoor->getTimeOut(), timedDoor->getAdapter());
    });
}

TEST(MockedDoorTest, TimerCallsTimeout) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(1);

    Timer timer;
    timer.tregister(1, &mockClient);
}

TEST_F(TimedDoorTest, AdapterLinksToTimedDoor) {
    DoorTimerAdapter adapter(*timedDoor);
    EXPECT_EQ(&adapter.getDoor(), timedDoor);
}

TEST_F(TimedDoorTest, DoorInitiallyLocked) {
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockingAndLockingDoorTwice) {
    timedDoor->unlock();
    timedDoor->lock();
    timedDoor->unlock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockingDoorAndThenTimingOut) {
    timedDoor->unlock();
    std::this_thread::sleep_for
    (std::chrono::seconds(timedDoor->getTimeOut() + 1));
    EXPECT_THROW({
        timer->tregister(timedDoor->getTimeOut(), timedDoor->getAdapter());
    }, std::runtime_error);
}
