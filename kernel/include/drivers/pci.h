#ifndef _PCI_DRIVER_
#define _PCI_DRIVER_

#include <stdint.h>

int8_t pciConfigReadByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
int16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
int32_t pciConfigReadLong(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

#endif
