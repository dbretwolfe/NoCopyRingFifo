#pragma once

#include <cstring>
#include <cstdint>
#include <span>
#include <vector>

template <typename T> class NoCopyRingFifo
{
public:
    // Struct to hold spans used to view or copy a block of data in the FIFO.
    // bool isSplit indicates whether the data segment is split between two spans due to the
    // segment wrapping around the end of the ring buffer memory space.
    // bool isValid indicates whether either span is valid and can be used at the time the
    // object is returned from a function.
    struct FifoSpans
    {
        std::span<T> span0;
        std::span<T> span1;
        bool isSplit;
        bool isValid;
    };

    NoCopyRingFifo(size_t fifoSize)
    {
        ringBuffer.resize(fifoSize);
        ringBufferSpan = std::span<T>(ringBuffer);
    }

    // Reserve a block of FIFO memory, returning a FifoSpans object.  Success or failure
    // is indicated by the isValid bool flag in the returned object.  Failure is caused by
    // insufficient reservable space for the attempted reserve size.
    FifoSpans Reserve(size_t reserveSize)
    {
        FifoSpans fifoSpans { .isSplit = false, .isValid = false };

        if (reserveSize > ReservableSize())
        {
            return fifoSpans;
        }

        fifoSpans = GetFifoSpans(&writeIndex, reserveSize, true);

        reserved += reserveSize;

        return fifoSpans;
    }

    // Commit a block of data to the FIFO.  This increases the amount of committed data
    // that is available to be read and decreases the amount of reserved data, both by the
    // commit size.  Failure is caused by insufficient reserved space for the commit.
    bool Commit(size_t commitSize)
    {
        if (commitSize > CommitableSize())
        {
            return false;
        }

        committed += commitSize;
        reserved -= commitSize;
    }

    inline size_t ReservableSize(void) const { return (ringBuffer->size() - (reserved + committed)); }
    inline size_t CommitableSize(void) const { return reserved; }
    inline size_t ReadableSize(void) const { return committed; }

    // Get a span of data starting at the current read index.
    // This does not increment the read index.
    FifoSpans GetReadSpans(size_t readSize) const
    {
        FifoSpans fifoSpans { .isSplit = false, .isValid = false };

        if (readSize > committed)
        {
            return fifoSpans;
        }

        fifoSpans = GetFifoSpans(&readIndex, readSize, false);

        return fifoSpans;
    }

    bool IncrementReadIndex(size_t readsize)
    {
        if (readsize > ReadableSize())
        {
            return false;
        }

        readIndex = (readIndex + readsize) % ringBuffer.size();
    }

    void ResetFifo(void)
    {
        readIndex = 0;
        writeIndex = 0;
        reserved = 0;
        committed = 0;
    }
    
private:

    FifoSpans GetFifoSpans(size_t &index, size_t length, bool incrementIndex)
    {
        FifoSpans fifoSpans { .isSplit = false, .isValid = false };
        const size_t remainingFifoSize = (ringBuffer->size() - index);

        if (length > ringBuffer->size())
        {
            return fifoSpans;
        }

        fifoSpans.isValid = true;

        if (length > remainingFifoSize)
        {
            fifoSpans.span0 = ringBufferSpan.subspan(index, remainingFifoSize);
            fifoSpans.span1 = ringBufferSpan.subspan(0, (length - remainingFifoSize));
            fifoSpans.isSplit = true;

            if (incrementIndex == true)
            {
                index = length - remainingFifoSize;
            }
        }
        else
        {
            fifoSpans.span0 = ringBufferSpan.subspan(writeIndex, length);

            if (incrementIndex == true)
            {
                index += length;
            }
        }

        return fifoSpans;
    }

    std::vector<T> ringBuffer;
    std::span<T> ringBufferSpan;
    size_t readIndex = 0;
    size_t writeIndex = 0;
    size_t reserved = 0;
    size_t committed = 0;
};