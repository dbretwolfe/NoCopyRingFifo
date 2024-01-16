#pragma once

#include <gtest/gtest.h>

#include "no_copy_ring_fifo.h"

class FifoTest : public testing::Test
{
protected:
    void SetUp() override
    {
        fifo = new NoCopyRingFifo<uint8_t>(maxFifoSize);
    }

    static constexpr size_t maxFifoSize = 4095;
    NoCopyRingFifo<uint8_t>* fifo = NULL;
};