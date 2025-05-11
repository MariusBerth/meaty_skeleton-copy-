//Driver pour la carte RTL8139

#include <stdint.h>
#include <drivers/pci.h>
#include <kernel/fb_tty.h>
#include <kernel/asm_functions.h>

uint16_t DEVICE_ID = 0x8139;
uint16_t VENDOR_ID = 0x10EC;

char rx_buffer[9708];

uint8_t device;
uint8_t bus;

uint32_t ioaddr;

void card_setup (void) {
	uint16_t device_bus = findDevice (DEVICE_ID, VENDOR_ID);
	
	if (device_bus == 0xFFFF) {	//La carte n'a pas été trouvée
		fb_writestring ("La carte reseau n'a pas ete trouvee");
		return ;}
	bus = (uint8_t) (device_bus & 0xFF);
	device = (uint8_t) (device_bus >> 8);

	uint16_t cmd_reg_pci = pciConfigReadWord (bus, device, 0, 0x04);
	pciConfigWriteWord (bus, device, 0, 0x04, cmd_reg_pci | 0x02);

	ioaddr = (pciConfigReadLong (bus, device, 0, 0x10)) & 0xFFFFFFFC;

	outb( ioaddr + 0x52, 0x0);
	outb( ioaddr + 0x37, 0x10);
 	while( (inb(ioaddr + 0x37) & 0x10) != 0) { }

	outportd(ioaddr + 0x30, (uintptr_t)rx_buffer); // send uint32_t memory location to RBSTART (0x30)
	
	outportl(ioaddr + 0x44, 0xf | (1 << 7)); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
						 // Pour savoir quels paquets accepter
}
