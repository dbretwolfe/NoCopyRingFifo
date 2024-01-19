#pragma once

#include <cstring>
#include <cstdint>
#include <span>
#include <vector>
#include <format>

template <typename T> class NoCopyRingFifo
{
public:
    
    // Class to hold spans used to view or copy a block of data in the FIFO.
    // A read or write to the FIFO may be split between 2 spans if it wraps around the end of the buffer.
    class DataBlock
    {
    public:
        DataBlock() : spans{ std::span<T>(), std::span<T>() } {}
        DataBlock(std::span<T>&& span0) : spans{ span0, std::span<T>() } {}
        DataBlock(std::span<T>&& span0, std::span<T>&& span1) : spans{ span0, span1 } {}

        inline bool isSplit(void) const { return (spans[1].empty() == false); }
        inline bool isValid(void) const { return (spans[0].empty() == false); }

        std::span<T> spans[2];
    };

    NoCopyRingFifo(size_t size) : maxSize(size)
    {
        ringBuffer.resize(size);
        ringBufferSpan = std::span<T>(ringBuffer);
    }

    // Reserve a block of FIFO memory, returning a FifoBlock object.
    // An exception is thrown if there is insufficient reservable space.
    DataBlock Reserve(size_t reserveSize)
    {
        if (reserveSize > ReservableSize())
        {
            throw std::overflow_error(
                std::format("Not enough free space in FIFO for reserve - requested {}, available {}",
                reserveSize,
                ReservableSize())
                );
        }

        reserved += reserveSize;

        return GetDataBlock(writeIndex, reserveSize);
    }

    // Commit a block of data to the FIFO.  This increases the amount of committed data that is
    // available to be read and decreases the amount of reserved data, both by the commit size.
    // An exception is throw if there is insufficient reserved space for the commit.
    void Commit(size_t commitSize)
    {
        if (commitSize > CommitableSize())
        {
            throw std::overflow_error(
                std::format("Not enough reserved space in FIFO for commit - requested {}, available {}", 
                commitSize,
                CommitableSize()
                )
                );
        }

        committed += commitSize;
        reserved -= commitSize;
    }

    inline size_t ReservableSize(void) const { return (ringBuffer.size() - (reserved + committed)); }
    inline size_t CommitableSize(void) const { return reserved; }
    inline size_t ReadableSize(void) const { return committed; }

    // Get a span of data starting at the current read index.
    // This does not increment the read index.
    DataBlock ReadBlock(size_t size)
    {
        if (size > committed)
        {
            throw std::underflow_error(
                std::format("Read larger than committed size - requested {}, available {}", size, committed)
                );
        }

        committed -= size;

        return GetDataBlock(readIndex, size);
    }

    void Reset(void)
    {
        readIndex = 0;
        writeIndex = 0;
        reserved = 0;
        committed = 0;
    }

    const size_t maxSize;
    
private:

    DataBlock GetDataBlock(size_t& index, size_t size)
    {
        if (size > ringBuffer.size())
        {
            throw std::overflow_error(
                std::format("Requested span size larger than FIFO size - requested {}, available {}", 
                size,
                ringBuffer.size()
                )
                );
        }
        else if (size == 0)
        {
            return DataBlock();
        }

        const size_t remainingBufferSize = (ringBuffer.size() - index);
        size_t oldIndex = index;
        index = (index + size) % ringBuffer.size();

        if (size > remainingBufferSize)
        {
            return DataBlock(
                ringBufferSpan.subspan(oldIndex, remainingBufferSize),
                ringBufferSpan.subspan(0, index)
                );
        }
        else
        {
            return DataBlock(ringBufferSpan.subspan(oldIndex, size));
        }
    }

    std::vector<T> ringBuffer;
    std::span<T> ringBufferSpan;
    size_t readIndex = 0;
    size_t writeIndex = 0;
    size_t reserved = 0;
    size_t committed = 0;
};