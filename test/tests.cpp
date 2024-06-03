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
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorTimeoutAfterLock) {
    timedDoor->unlock();
    timedDoor->lock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 5));
    EXPECT_TRUE(timedDoor->isDoorOpened());
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

TEST(MockedDoorTest, TimerCallsTimeoutMultipleTimes) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(testing::AtLeast(1));

    Timer timer;
    timer.tregister(1, &mockClient);
    timer.tregister(1, &mockClient);
    timer.tregister(1, &mockClient);
}

TEST(MockedDoorTest, TimerCallsTimeoutWithDifferentTimeouts) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(testing::AtLeast(1));

    Timer timer;
    timer.tregister(1, &mockClient);
    timer.tregister(2, &mockClient);
    timer.tregister(3, &mockClient);
}

TEST(MockedDoorTest, TimerCallsTimeoutAfterDoorUnlock) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(testing::AtLeast(1));

    Timer timer;
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);

    door.unlock();
    timer.tregister(1, &mockClient);
}

TEST(MockedDoorTest, TimerCallsTimeoutAfterDoorLock) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(testing::AtLeast(1));

    Timer timer;
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);

    door.unlock();
    door.lock();
    timer.tregister(1, &mockClient);
}

TEST(MockedDoorTest, TimerCallsTimeoutAfterDoorUnlockAndLock) {
    MockTimerClient mockClient;
    EXPECT_CALL(mockClient, Timeout()).Times(testing::AtLeast(1));

    Timer timer;
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);

    door.unlock();
    door.lock();
    door.unlock();
    timer.tregister(1, &mockClient);
}
