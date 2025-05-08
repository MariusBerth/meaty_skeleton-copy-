#ifndef _PCI_DRIVER_
#define _PCI_DRIVER_

#include <stdint.h>

int8_t pciConfigReadByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
int16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
int32_t pciConfigReadLong(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

void pciConfigWriteByte (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t data);
void pciConfigWriteWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data);
void pciConfigWriteLong (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data);

uint16_t findDevice (uint16_t deviceID, uint16_t vendorID);


#endif
