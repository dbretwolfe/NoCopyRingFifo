#include <format>

#include <gtest/gtest.h>

#include "fifo_test_fixture.h"


TEST_F(FifoTest, Reset)
{
    fifo->Reset();

    // Fifo should be empty after reset.
    EXPECT_EQ(fifo->CommitableSize(), 0);
    EXPECT_EQ(maxFifoSize, fifo->ReservableSize());

    // Try to commmit, should fail because no space has been reserved.
    EXPECT_THROW(fifo->Commit(1), std::overflow_error);
}

// Test filling the FIFO from a reset state with reserve, write, commit, and read of one data element at a time.
TEST_F(FifoTest, ReserveWriteCommitReadSingle)
{
    fifo->Reset();

    fifoDataType testVector[maxFifoSize] = {0};

    // Reserve until full, testing size along the way.
    for (int i = 0; i < maxFifoSize; i++)
    {
        SCOPED_TRACE(std::format("Reserve loop iteration {}\r\n", i));

        EXPECT_EQ(fifo->ReservableSize(), (maxFifoSize - i));
        EXPECT_EQ(fifo->CommitableSize(), i);

        NoCopyRingFifo<fifoDataType>::DataBlock dataBlock = fifo->Reserve(1);

        dataBlock.spans[0][0] = i;
        testVector[i] = i;
    }

    // Try to reserve, should throw due to FIFO being fully reserved.
    EXPECT_THROW(fifo->Reserve(1), std::overflow_error);

    // Commit until full, testing size along the way.
    for (int i = 0; i < maxFifoSize; i++)
    {
        SCOPED_TRACE(std::format("Commit loop iteration {}\r\n", i));

        EXPECT_EQ(fifo->ReservableSize(), 0);
        EXPECT_EQ(fifo->CommitableSize(), (maxFifoSize - i));

        fifo->Commit(1);
    }

    // Try to commit, should throw due to FIFO being fully committed.
    EXPECT_THROW(fifo->Commit(1), std::overflow_error);

    // Read until empty.
    for (int i = 0; i < maxFifoSize; i++)
    {
        SCOPED_TRACE(std::format("Read loop iteration {}\r\n", i));

        EXPECT_EQ(fifo->ReservableSize(), i);
        EXPECT_EQ(fifo->CommitableSize(), 0);

        fifoDataType dataOut = fifo->ReadBlock(1).spans[0][0];
        
        EXPECT_EQ(testVector[i], dataOut);
    }

    // Try to read, should throw because FIFO is empty.
    EXPECT_THROW(fifo->ReadBlock(1), std::underflow_error);
}

// Test reserves of varying block sizes.
TEST_F(FifoTest, Reserve)
{
    for (int i = 0; i < maxFifoSize; i++)
    {
        SCOPED_TRACE(std::format("Reserve loop iteration {}\r\n", i));

        fifo->Reset();

        NoCopyRingFifo<fifoDataType>::DataBlock dataBlock;

        EXPECT_NO_THROW(dataBlock = fifo->Reserve(i));

        EXPECT_EQ(dataBlock.spans[0].size(), i);
        EXPECT_EQ(fifo->ReservableSize(), (maxFifoSize - i));
        EXPECT_EQ(fifo->CommitableSize(), i);
    }
}

// Test commits of varying block sizes.
TEST_F(FifoTest, Reserve)
{
    for (int i = 0; i < maxFifoSize; i++)
    {
        SCOPED_TRACE(std::format("Commit loop iteration {}\r\n", i));

        fifo->Reset();

        EXPECT_NO_THROW(fifo->Reserve(maxFifoSize));

        EXPECT_NO_THROW(fifo->Commit(i));

        EXPECT_EQ(fifo->ReservableSize(), 0);
        EXPECT_EQ(fifo->CommitableSize(), maxFifoSize - i);
    }
}