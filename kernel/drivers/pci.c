#include <stdint.h>
#include <kernel/asm_functions.h>

uint32_t CONFIG_ADDRESS = 0xCF8;
uint32_t CONFIG_DATA = 0xCFC;

uint8_t pciConfigReadByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint8_t tmp = 0;

    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint8_t)((inl(CONFIG_DATA) >> ((offset & 3) * 8)) & 0xFF);
    return tmp;
}
//https://wiki.osdev.org/PCI
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

uint32_t pciConfigReadLong(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    // On suppose que les offsets sont alignés sur la taille de la sortie voulue
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    return inl(CONFIG_DATA);
}

void pciConfigWriteByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t data) {
    // On suppose que les offsets sont alignés sur la taille de la sortie voulue
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;

    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    tmp = inl(CONFIG_DATA);
    tmp &= ~(0xFF << ((offset & 3) * 8));
    tmp |= ((uint32_t)data) << ((offset & 3) * 8);
    
    // A tester pour voir si nescessaire
    outl(CONFIG_ADDRESS, address);

    outl(CONFIG_DATA, tmp); 
}

void pciConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data) {
    // On suppose que les offsets sont alignés sur la taille de la sortie voulue
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;

    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    tmp = inl(CONFIG_DATA);
    tmp &= ~(0xFF << ((offset & 2) * 8));
    tmp |= ((uint32_t)data) << ((offset & 2) * 8);
    
    // A tester pour voir si nescessaire
    outl(CONFIG_ADDRESS, address);

    outl(CONFIG_DATA, tmp); 
}

void pciConfigWriteLong(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data) {
    // On suppose que les offsets sont alignés sur la taille de la sortie voulue
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(CONFIG_ADDRESS, address);
    outl(CONFIG_DATA, data);
}
