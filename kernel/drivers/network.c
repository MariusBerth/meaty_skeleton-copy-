/* Adapté des drivers suivants (certaines parties ont été directement récupérées)*/
/*
 * rtl8139.c : U-Boot driver for the RealTek RTL8139
 *
 * Masami Komiya (mkomiya@sonare.it)
 *
 * Most part is taken from rtl8139.c of etherboot
 *
 */

/* rtl8139.c - etherboot driver for the Realtek 8139 chipset

  ported from the linux driver written by Donald Becker
  by Rainer Bawidamann (Rainer.Bawidamann@informatik.uni-ulm.de) 1999

  This software may be used and distributed according to the terms
  of the GNU Public License, incorporated herein by reference.

  changes to the original driver:
  - removed support for interrupts, switching to polling mode (yuck!)
  - removed support for the 8129 chip (external MII)

*/
//Driver pour la carte RTL8139

#include <stdint.h>
#include <string.h>
#include <drivers/pci.h>
#include <kernel/fb_tty.h>
#include <kernel/asm_functions.h>

#define TX_BUF_SIZE 1514
#define TX_DESC_NUM_MOD 16
#define TX_FIFO_THRESH 256
#define RX_BUF_LEN_IDX 0	/* 0, 1, 2 is allowed - 8,16,32K rx buffer */
#define RX_BUF_LEN_MOD (8192 << RX_BUF_LEN_IDX)
#define RX_BUF_LEN (RX_BUF_LEN_MOD + 1500)

#define ETH_ZLEN 60
#define ETH_FRAME_LEN 1514

enum RTL8139_registers {
	MAC0=0,			/* Ethernet hardware address. */
	MAR0=8,			/* Multicast filter. */
	TxStatus0=0x10,		/* Transmit status (four 32bit registers). */
	TxAddr0=0x20,		/* Tx descriptors (also four 32bit). */
	RxBuf=0x30, RxEarlyCnt=0x34, RxEarlyStatus=0x36,
	ChipCmd=0x37, RxBufPtr=0x38, RxBufAddr=0x3A,
	IntrMask=0x3C, IntrStatus=0x3E,
	TxConfig=0x40, RxConfig=0x44,
	Timer=0x48,		/* general-purpose counter. */
	RxMissed=0x4C,		/* 24 bits valid, write clears. */
	Cfg9346=0x50, Config0=0x51, Config1=0x52,
	TimerIntrReg=0x54,	/* intr if gp counter reaches this value */
	MediaStatus=0x58,
	Config3=0x59,
	MultiIntr=0x5C,
	RevisionID=0x5E,	/* revision of the RTL8139 chip */
	TxSummary=0x60,
	MII_BMCR=0x62, MII_BMSR=0x64, NWayAdvert=0x66, NWayLPAR=0x68,
	NWayExpansion=0x6A,
	DisconnectCnt=0x6C, FalseCarrierCnt=0x6E,
	NWayTestReg=0x70,
	RxCnt=0x72,		/* packet received counter */
	CSCR=0x74,		/* chip status and configuration register */
	PhyParm1=0x78,TwisterParm=0x7c,PhyParm2=0x80,	/* undocumented */
	/* from 0x84 onwards are a number of power management/wakeup frame
	 * definitions we will probably never need to know about.  */
};

/* Interrupt register bits, using my own meaningful names. */
enum IntrStatusBits {
	PCIErr=0x8000, PCSTimeout=0x4000, CableLenChange= 0x2000,
	RxFIFOOver=0x40, RxUnderrun=0x20, RxOverflow=0x10,
	TxErr=0x08, TxOK=0x04, RxErr=0x02, RxOK=0x01,
};

enum ChipCmdBits {
	CmdReset=0x10, CmdRxEnb=0x08, CmdTxEnb=0x04, RxBufEmpty=0x01, 
};

enum RxStatusBits {
	RxMulticast=0x8000, RxPhysical=0x4000, RxBroadcast=0x2000,
	RxBadSymbol=0x0020, RxRunt=0x0010, RxTooLong=0x0008, RxCRCErr=0x0004,
	RxBadAlign=0x0002, RxStatusOK=0x0001,
};

#define DEVICE_ID 0x8139
#define VENDOR_ID 0x10EC

static unsigned char tx_buffer[TX_BUF_SIZE] __attribute__((aligned(4)));
static unsigned char rx_ring[RX_BUF_LEN+16] __attribute__((aligned(4)));

static uint8_t device;
static uint8_t bus;

static uint32_t cur_tx;
static uint32_t cur_rx;

static uint32_t ioaddr;

void card_setup (void) {
	uint16_t device_bus = findDevice (DEVICE_ID, VENDOR_ID);
	
	if (device_bus == 0xFFFF) {	//La carte n'a pas été trouvée
		fb_writestring ("La carte reseau n'a pas ete trouvee");
		return ;}
	bus = (uint8_t) (device_bus & 0xFF);
	device = (uint8_t) (device_bus >> 8);

	uint16_t cmd_reg_pci = pciConfigReadWord (bus, device, 0, 0x04);
	pciConfigWriteWord (bus, device, 0, 0x04, cmd_reg_pci | 0x04);

	ioaddr = (pciConfigReadLong (bus, device, 0, 0x10)) & 0xFFFFFFFC;

	outb( ioaddr + Config1, 0x0);
	outb( ioaddr + ChipCmd, 0x10);
 	while( (inb(ioaddr + ChipCmd) & 0x10) != 0) { }

	outl(ioaddr + RxBuf, (uint32_t)rx_ring); // on envoie la position du buffer à la carte 
	
	outw(ioaddr + IntrMask, 0x0000); 	//La carte réseau ne pourra pas reçevoir d'interrupts
	outl(ioaddr + RxConfig, 0xf | (1 << 7)); 	// (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
							// Pour savoir quels paquets accepter
	outb(ioaddr + ChipCmd, 0x0C);		// Sets the RE and TE bits high
	
	cur_tx = 0;
	cur_rx = 0;
	/*fb_writestring ("position du buffer dans la carte: ");
	fb_writelong (inl(ioaddr + RxBuf));
	fb_writestring ("\nposition de la tete d'ecriture de la carte: ");
	fb_writelong (inl(ioaddr + RxBufPtr));
	fb_writestring ("\nposition du buffer dans le code: ");
	fb_writelong ((uint32_t) rx_ring);
	fb_putchar ('\n');*/
}

int rtl_transmit(volatile void*packet, size_t length) {
	uint16_t status;
        uint32_t txstatus;
	size_t len = length;
	

	volatile_memcpy ( tx_buffer, packet, length);

	/* Note: RTL8139 doesn't auto-pad, send minimum payload (another 4
	 * bytes are sent automatically for the FCS, totalling to 64 bytes). */
	while (len < ETH_ZLEN) {
		tx_buffer[len++] = '\0';
	}

	outl( ioaddr + TxAddr0 + cur_tx, (uint32_t)tx_buffer);
	outl( ioaddr + TxStatus0 + cur_tx, 
			((TX_FIFO_THRESH<<11) & 0x003f0000) | len);

	status = inw (ioaddr + IntrStatus) & ( TxOK | TxErr | PCIErr );
	outw ( ioaddr + IntrStatus, status );
	while (! status) {
		status = inw (ioaddr + IntrStatus) & ( TxOK | TxErr | PCIErr );
		outw ( ioaddr + IntrStatus, status );
	};

	txstatus = inl(ioaddr + TxStatus0 + cur_tx);

	if (status & TxOK) {
		cur_tx = (cur_tx + 4) % TX_DESC_NUM_MOD;
		//fb_writestring ("l'envoi du packet a reussi\n");
		
		return length; }
	else {
		fb_writestring ("L'envoi du paquet a raté, réinitialisation de\
				la carte réseau\n");
		card_setup ();
		return 0;	// On supposera que personne n'essayera d'envoyer un paquet
				// de taille 0 ainsi un retour de 0 signifie une erreur.
	};
}

int rtl_poll (volatile void* DstBuf) {
	//Le pointeur de destination doit faire au moins 1514 de taille
	uint32_t status;
	uint32_t rx_size, rx_status;
	uint32_t length;

	if (inb(ioaddr + ChipCmd) & RxBufEmpty) {
		fb_writestring ("aucun paquet recu\n");
		return 0;
	}
	
	status = inw(ioaddr + IntrStatus);
	/* See below for the rest of the interrupt acknowledges.  */
	outw(ioaddr + IntrStatus, status & ~(RxFIFOOver | RxOverflow | RxOK));

	rx_status = *(uint32_t*) (rx_ring + cur_rx);
	rx_size = rx_status >> 16;
	rx_status &= 0xFFFF;


	if ((rx_status & (RxBadSymbol|RxRunt|RxTooLong|RxCRCErr|RxBadAlign)) ||
	    (rx_size < ETH_ZLEN) || (rx_size > ETH_FRAME_LEN + 4)) {
		fb_writestring("erreur dans la reception d'un paquet, reinitialisation de la carte\nstatut:");
		fb_writeword ((uint16_t)status);
		fb_writestring ("\ntaille:");
		fb_writeword (rx_size);
		fb_putchar ('\n');
		int i;
		for (i=0; i<RX_BUF_LEN; i++){
			if (rx_ring[i]) {
				fb_writestring ("information trouvee dans le buffer de reception");
			};
		};
		card_setup (); /* this clears all interrupts still pending */
		return 0;
	};

	if (!(rx_status & RxStatusOK)) {
		fb_writestring("erreur dans la reception d'un paquet, reinitialisation de la carte\n");
		card_setup (); /* this clears all interrupts still pending */
		return 0;
	};

	//A partir d'ici, un paquet a bien été recu
	length = rx_size - 4;	//On ignore la checksum
	
	//pas besoin de faire de distinction car les paquets sont écrits continuements
	//quitte à dépasser le buffer en anneau
	volatile_memcpy(DstBuf, (rx_ring + cur_rx + 4), length);
	
	//fb_writestring("paquet recu");
	
	cur_rx = ((cur_rx + rx_size + 4 + 3) & ~3) % RX_BUF_LEN_MOD;
	outw(ioaddr + RxBufPtr, cur_rx - 16);
	/* See RTL8139 Programming Guide V0.1 for the official handling of
	 * Rx overflow situations.  The document itself contains basically no
	 * usable information, except for a few exception handling rules.  */
	outw(ioaddr + IntrStatus, status & (RxFIFOOver | RxOverflow | RxOK));
	return length;
};

int rtl_poll_wait (volatile void* DstBuf) {
	//Le pointeur de destination doit faire au moins 1514 de taille
	uint32_t status;
	uint32_t rx_size, rx_status;
	uint32_t length;

	while (inb(ioaddr + ChipCmd) & RxBufEmpty);
	
	status = inw(ioaddr + IntrStatus);
	/* See below for the rest of the interrupt acknowledges.  */
	outw(ioaddr + IntrStatus, status & ~(RxFIFOOver | RxOverflow | RxOK));

	rx_status = *(uint32_t*) (rx_ring + cur_rx);
	rx_size = rx_status >> 16;
	rx_status &= 0xFFFF;


	if ((rx_status & (RxBadSymbol|RxRunt|RxTooLong|RxCRCErr|RxBadAlign)) ||
	    (rx_size < ETH_ZLEN) || (rx_size > ETH_FRAME_LEN + 4)) {
		fb_writestring("erreur dans la reception d'un paquet, reinitialisation de la carte\nstatut:");
		fb_writeword ((uint16_t)status);
		fb_writestring ("\ntaille:");
		fb_writeword (rx_size);
		fb_putchar ('\n');
		int i;
		for (i=0; i<RX_BUF_LEN; i++){
			if (rx_ring[i]) {
				fb_writestring ("information trouvee dans le buffer de reception");
			};
		};
		card_setup (); /* this clears all interrupts still pending */
		return 0;
	};

	if (!(rx_status & RxStatusOK)) {
		fb_writestring("erreur dans la reception d'un paquet, reinitialisation de la carte\n");
		card_setup (); /* this clears all interrupts still pending */
		return 0;
	};

	//A partir d'ici, un paquet a bien été recu
	length = rx_size - 4;	//On ignore la checksum
	
	//pas besoin de faire de distinction car les paquets sont écrits continuements
	//quitte à dépasser le buffer en anneau
	volatile_memcpy(DstBuf, (rx_ring + cur_rx + 4), length);
	
	//fb_writestring("paquet recu");
	
	cur_rx = ((cur_rx + rx_size + 4 + 3) & ~3) % RX_BUF_LEN_MOD;
	outw(ioaddr + RxBufPtr, cur_rx - 16);
	/* See RTL8139 Programming Guide V0.1 for the official handling of
	 * Rx overflow situations.  The document itself contains basically no
	 * usable information, except for a few exception handling rules.  */
	outw(ioaddr + IntrStatus, status & (RxFIFOOver | RxOverflow | RxOK));
	return length;
};
