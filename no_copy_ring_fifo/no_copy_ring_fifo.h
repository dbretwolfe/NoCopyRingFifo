/*
*   NoCopyRingFifo class
*
*   This class defines a type of FIFO that provides the user with Span objects that provide a readable and writable
*   view into chunks of FIFO memory without copying data in or out.  This is useful when delegating the actual read and
*   write operations for asynchronous/overlapped IO operations.
*
*   Writing to the FIFO happens in 3 stages - First a block of data in the FIFO is reserved, then at some later time
*   data is written.  Once the write is complete, the written data is committed to the FIFO.
*
*   Once data is comitted to the FIFO, it is immediately available to read.
*
*   The underlying memory is a single contiguous block, and read and write operations wrap around the ends.  It is
*   therefore possible that a read or write could involve two different copy operations on seperate sections of memory.
*   The DataBlock class defined here contains two spans to cover the case of a wraparound.
*/

#pragma once

#include <cstring>
#include <cstdint>
#include <span>
#include <vector>
#include <format>

namespace FifoTemplates
{
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
            _ringBuffer.resize(size);
            _ringBufferSpan = std::span<T>(_ringBuffer);
        }

        // Reserve a block of FIFO memory, returning a FifoBlock object.
        // An exception is thrown if there is insufficient reservable space.
        DataBlock Reserve(size_t size)
        {
            if (size > ReservableSize())
            {
                throw std::overflow_error(
                    std::format("Not enough free space in FIFO for reserve - requested {}, available {}",
                    size,
                    ReservableSize())
                    );
            }

            _reserved += size;

            return GetDataBlock(_writeIndex, size);
        }

        // Commit a block of data to the FIFO.  This increases the amount of committed data that is
        // available to be read and decreases the amount of reserved data, both by the commit size.
        // An exception is throw if there is insufficient reserved space for the commit.
        void Commit(size_t size)
        {
            if (size > CommitableSize())
            {
                throw std::overflow_error(
                    std::format("Not enough reserved space in FIFO for commit - requested {}, available {}", 
                    size,
                    CommitableSize()
                    )
                    );
            }

            _committed += size;
            _reserved -= size;
        }

        inline size_t ReservableSize(void) const { return (_ringBuffer.size() - (_reserved + _committed)); }
        inline size_t CommitableSize(void) const { return _reserved; }
        inline size_t ReadableSize(void) const { return _committed; }

        // Get a block of comitted data to read.
        DataBlock ReadBlock(size_t size)
        {
            if (size > _committed)
            {
                throw std::underflow_error(
                    std::format("Read larger than committed size - requested {}, available {}", size, _committed)
                    );
            }

            _committed -= size;

            return GetDataBlock(_readIndex, size);
        }

        void Reset(void)
        {
            _readIndex = 0;
            _writeIndex = 0;
            _reserved = 0;
            _committed = 0;
        }

        const size_t maxSize;
        
    private:
        // Get a block of data starting at the specified index.  This is used by both the Reserve and ReadBlock functions.
        DataBlock GetDataBlock(size_t& index, size_t size)
        {
            if (size > _ringBuffer.size())
            {
                throw std::overflow_error(
                    std::format("Requested span size larger than FIFO size - requested {}, available {}", 
                    size,
                    _ringBuffer.size()
                    )
                    );
            }
            else if (size == 0)
            {
                return DataBlock();
            }

            const size_t remainingBufferSize = (_ringBuffer.size() - index);
            size_t oldIndex = index;
            index = (index + size) % _ringBuffer.size();

            if (size > remainingBufferSize)
            {
                return DataBlock(
                    _ringBufferSpan.subspan(oldIndex, remainingBufferSize),
                    _ringBufferSpan.subspan(0, index)
                    );
            }
            else
            {
                return DataBlock(_ringBufferSpan.subspan(oldIndex, size));
            }
        }

        std::vector<T> _ringBuffer;
        std::span<T> _ringBufferSpan;
        size_t _readIndex = 0;
        size_t _writeIndex = 0;
        size_t _reserved = 0;
        size_t _committed = 0;
    };
}