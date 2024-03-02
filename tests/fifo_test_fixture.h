#pragma once

#include <gtest/gtest.h>

#include "no_copy_ring_fifo.h"

using namespace FifoTemplates;

typedef uint8_t fifoDataType;

class FifoTest : public testing::Test
{
protected:
    void SetUp() override
    {
    }
    
    std::vector<fifoDataType> GetTestVector(int size);

    static constexpr size_t maxFifoSize = 10;
    NoCopyRingFifo<uint8_t> fifo = NoCopyRingFifo<uint8_t>(maxFifoSize);
};