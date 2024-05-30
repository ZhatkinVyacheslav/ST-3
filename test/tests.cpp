// Copyright 2024 Sokolova Daria

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <chrono> // NOLINT [build/c++11]
#include <thread> // NOLINT [build/c++11]

#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor door;
    MockTimerClient mockClient;
    Timer timer;

    TimedDoorTest() : door(1), timer() {}

    void SetUp() override {
        timer.tregister(door.getTimeOut(), &mockClient);
    }

    void TearDown() override {
        testing::Mock::
            VerifyAndClearExpectations(&mockClient);
    }
};

TEST_F(TimedDoorTest, UnlockDoor) {
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, LockDoor) {
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, InitLockDoor) {
    EXPECT_FALSE(door.isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpenedDoorAgain) {
    door.unlock();
    EXPECT_THROW(door.unlock(), std::logic_error);
}

TEST_F(TimedDoorTest, LockClosedDoorAgain) {
    EXPECT_THROW(door.lock(), std::logic_error);
}

TEST_F(TimedDoorTest, ClosedBeforeTimeoutNoThrow) {
    EXPECT_NO_THROW(door.throwState());
}

TEST_F(TimedDoorTest, OpenedExceptionThrow) {
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, LockedAfterTimeoutNoException) {
    std::this_thread::sleep_for(std::
        chrono::seconds(door.getTimeOut() + 1));
    EXPECT_NO_THROW(door.throwState());
}

TEST_F(TimedDoorTest, OpenAfterTimeoutThrowsException) {
    std::this_thread::sleep_for(std::chrono::
        seconds(door.getTimeOut() + 1));
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, UnlockDoorTimeoutException) {
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds
    (door.getTimeOut()));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimerCallsClientTimeout) {
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(door.getTimeOut(), &mockClient);
    std::this_thread::sleep_for(std::chrono::
        seconds(door.getTimeOut() + 1));
}
