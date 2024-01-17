#pragma once

#include <gtest/gtest.h>

#include "no_copy_ring_fifo.h"

typedef uint8_t fifoDataType;

class FifoTest : public testing::Test
{
protected:
    void SetUp() override
    {
        fifo = new NoCopyRingFifo<uint8_t>(maxFifoSize);
    }

    static constexpr size_t maxFifoSize = 10;
    NoCopyRingFifo<uint8_t>* fifo = NULL;
};