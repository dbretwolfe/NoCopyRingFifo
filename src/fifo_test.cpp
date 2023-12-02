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

    std::vector<uint8_t> fifoVec = {0,1,2,3,4};
    std::span<uint8_t> fifoSpan = std::span<uint8_t>(fifoVec);

    spans.span0 = std::span<uint8_t>(fifoSpan.subspan(3, 2));
    spans.span1 = std::span<uint8_t>(fifoSpan.subspan(0, 3));

    printMe(spans.span0);
    printMe(spans.span1);

    delete ringfifo;
    
    std::cout << __cplusplus;
    return 0;
}