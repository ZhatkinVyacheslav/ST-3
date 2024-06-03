// Copyright 2024 Zhatkin Vyacheslav

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]
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

TEST_F(TimedDoorTest, DoorTimeoutAfterUnlock) {
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 5));
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorTimeoutAfterLock) {
    timedDoor->lock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 5));
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorTimeoutAfterUnlockAndLock) {
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() - 1));
    timedDoor->lock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 1));
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorTimeoutAfterMultipleUnlocks) {
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 3));
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 3));
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 3));
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeOpenedAfterUnlock) {
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeClosedAfterLock) {
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeOpenedAfterMultipleUnlocks) {
    timedDoor->unlock();
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeClosedAfterMultipleLocks) {
    timedDoor->unlock();
    timedDoor->lock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeOpenedAndClosed) {
    timedDoor->unlock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    int expectedTimeout = 2;
    EXPECT_EQ(timedDoor->getTimeOut(), expectedTimeout);
}