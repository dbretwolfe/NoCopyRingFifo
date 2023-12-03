#pragma once

#include <cstring>
#include <cstdint>
#include <span>
#include <vector>
#include <format>



template <typename T> class NoCopyRingFifo
{
public:
    
    // Struct to hold spans used to view or copy a block of data in the FIFO.
    struct FifoSpans
    {
        inline bool isSplit(void) const { return (span1.empty() == false); }
        inline bool isValid(void) const { return (span0.empty() == false); }

        std::span<T> span0;
        std::span<T> span1;
    };

    NoCopyRingFifo(size_t fifoSize)
    {
        ringBuffer.resize(fifoSize);
        ringBufferSpan = std::span<T>(ringBuffer);
    }

    // Reserve a block of FIFO memory, returning a FifoSpans object.
    // An exception is thrown if there is insufficient reservable space.
    FifoSpans Reserve(size_t reserveSize)
    {
        if (reserveSize > ReservableSize())
        {
            throw std::length_error(
                std::format("Not enough free space in FIFO for reserve - requested {}, available {}",
                reserveSize,
                ReservableSize())
                );
        }

        reserved += reserveSize;

        return GetFifoSpans(&writeIndex, reserveSize, true);;
    }

    // Commit a block of data to the FIFO.  This increases the amount of committed data that is
    // available to be read and decreases the amount of reserved data, both by the commit size.
    // An exception is throw if there is insufficient reserved space for the commit.
    void Commit(size_t commitSize)
    {
        if (commitSize > CommitableSize())
        {
            throw std::length_error(
                std::format("Not enough reserved space in FIFO for commit - requested {}, available {}", 
                commitSize,
                CommitableSize()
                )
                );
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
        if (readSize > committed)
        {
            throw std::length_error(
                std::format("Read larger than committed size - requested {}, available {}", 
                readSize,
                committed
                )
                );
        }

        return GetFifoSpans(&readIndex, readSize, false);
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
        if (length > ringBuffer->size())
        {
            throw std::length_error(
                std::format("Requested span length larger than FIFO size - requested {}, available {}", 
                length,
                ringBuffer->size()
                )
                );
        }

        FifoSpans fifoSpans { 0 };
        const size_t remainingFifoSize = (ringBuffer->size() - index);

        if (length > remainingFifoSize)
        {
            fifoSpans.span0 = ringBufferSpan.subspan(index, remainingFifoSize);
            fifoSpans.span1 = ringBufferSpan.subspan(0, (length - remainingFifoSize));

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