#include <format>

#include <gtest/gtest.h>

#include "fifo_test_fixture.h"

TEST_F(FifoTest, Reset)
{
    fifo->Reset();

    // Fifo should be empty after reset.
    EXPECT_EQ(0, fifo->CommitableSize());
    EXPECT_EQ(maxFifoSize, fifo->ReservableSize());
}

/*
TEST_F(FifoTestCpp, PushPop)
{
    fifo.Reset();

    uint8_t testVector[maxFifoSize] = {0};

    // Try to pop, should fail because FIFO is empty.
    uint8_t dataOut = 0;
    EXPECT_THROW(fifo.Pop(), std::underflow_error);

    // Push until full, testing size along the way.
    for (int i = 0; i < maxFifoSize; i++)
    {
        SCOPED_TRACE(std::format("Push loop iteration {}\r\n", i));
        fifo.Push(i);
        testVector[i] = i;

        EXPECT_EQ((i + 1), fifo.GetSize());
        EXPECT_EQ((maxFifoSize - (i + 1)), fifo.GetSizeRemaining());
    }

    // Try to push, should throw due to FIFO being full.
    EXPECT_THROW(fifo.Push(0), std::overflow_error);

    // Push until full, testing size along the way.
    for (int i = (maxFifoSize - 1); i >= 0; i--)
    {
        SCOPED_TRACE(std::format("Pop loop iteration {}\r\n", i));
        dataOut = 0;
        dataOut = fifo.Pop();
        EXPECT_EQ(testVector[maxFifoSize - (i + 1)], dataOut);

        EXPECT_EQ(i, fifo.GetSize());
        EXPECT_EQ((maxFifoSize - i), fifo.GetSizeRemaining());
    }

    // Try to pop, should fail because FIFO is empty.
    EXPECT_THROW(fifo.Pop(), std::underflow_error);
}

*/
