#include <string.h>

volatile void* volatile_memcpy(volatile void* dstptr, volatile void* srcptr, size_t size) {
        unsigned char* dst = (unsigned char*) dstptr;
        const unsigned char* src = (const unsigned char*) srcptr;
        for (size_t i = 0; i < size; i++)
                dst[i] = src[i];
        return dstptr;
}
