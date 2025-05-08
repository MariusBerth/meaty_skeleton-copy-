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

uint16_t getVendorID (uint8_t bus, uint8_t slot, uint8_t func) {
	return pciConfigReadWord (bus, slot, func, 0);
}

uint16_t getDeviceID (uint8_t bus, uint8_t slot, uint8_t func) {
	return pciConfigReadWord (bus, slot, func, 0x2);
}

uint32_t getVendorAndDeviceID (uint8_t bus, uint8_t slot, uint8_t func) {
	return pciConfigReadLong (bus slot func, 0);
}

uint16_t findDevice (uint16_t deviceID, uint16_t vendorID) {
	//Renvoie le bus dans l'octet de poids faible et le slot dans l'octet de poids faible
	//Une valeur de retour de 0xFFFF indique que l'appareil n'a pas pu être trouvé
	//(Cela est correct car il n'y a que 32 slots et donc 0xFFFF n'est pas une valeur valide
	//pour slot, octet) 
	
	uint16_t bus;
	uint8_t device;
	uint32_t id = (((uint32_t) deviceID) << 16) | (uint32_t) vendorID;

	for (bus = 0; bus < 0x0100; bus++) {
		for (device; device < 0x10; device++) {
			if (id == getVendorAndDeviceID (bus, device, 0)) {
				return (((uint16_t) device) << 8) | bus;
			};
		};
	};
	return 0xFFFF;
}


