#include "no_copy_ring_fifo.h"
#include <iostream>

#define NUM_FIFO_BYTES (1024)


void printMe(std::span<uint8_t> container) {
    
    std::cout << "container.size(): " << container.size() << std::endl;
    
    for(auto item : container) 
    {
        std::cout << +item << ' ' << std::flush;
    }

    std::cout << "\n\n";
}

int main()
{
    
    NoCopyRingFifo<uint8_t>* ringfifo = new NoCopyRingFifo<uint8_t>(NUM_FIFO_BYTES);

    NoCopyRingFifo<uint8_t>::FifoSpans spans;

    delete ringfifo;
    
    return 0;
}