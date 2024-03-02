#pragma once

#include <gtest/gtest.h>

#include "no_copy_ring_fifo.h"

typedef uint8_t fifoDataType;

class FifoTest : public testing::Test
{
protected:
    void SetUp() override
    {
    }
    
    std::vector<fifoDataType> GetTestVector(int size);

    static constexpr size_t maxFifoSize = 10;
    FifoTemplates::NoCopyRingFifo<uint8_t> fifo = FifoTemplates::NoCopyRingFifo<uint8_t>(maxFifoSize);
};