#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <tty.h>
#include <stdio.h>

static uint32_t* FB_ADDR;	//Voir pour rendre ça compatible avec le
				//fait que l'on peut nous donner un bpp
				//différent de 32.
static uint32_t FB_PITCH;
static uint32_t FB_WIDTH;
static uint32_t FB_HEIGHT;
static uint8_t FB_BPP;		//Attention: ici on stockera le nombre
				//d'octets par pixel, et non le nombre de bits
static uint8_t FB_TYPE;

static uint8_t FB_RED_FIELDPOS;	//TODO: rendre possible l'interprétattion
static uint8_t FB_RED_MASKSIZE;	//des autres types de couleurs
static uint8_t FB_GREEN_FIELDPOS;
static uint8_t FB_GREEN_MASKSIZE;
static uint8_t FB_BLUE_FIELDPOS;
static uint8_t FB_BLUE_MASKSIZE;


void read_multiboot_struct (void * multiboot_struct) {
	static uint32_t struct_size = multiboot_struct[0];
	uint32_t index = 8;
	
	while ( index < stuct_size && uint32_t multiboot_struct[index] != 8 ) {
		index = index + uint32_t multiboot_struct[index + 4];
	};

	if ( index >= struct_size ) {
		terminal_initialise ();
		printf("Couldn't find any info relative to the framebuffer in the
				multiboot structure,\n now hanging...");
		while true ;};

	FB_ADDR = (uint32_t*) multiboot_struct[index + 8];
	FB_PITCH = multiboot_struct[index + 16];
       	FB_WIDTH = multiboot_struct[index + 20];
	FB_HEIGHT = multiboot_struct[index + 24];
	FB_BPP = multiboot_struct[index + 28] >> 3;
	FB_TYPE = multiboot_struct[index + 29];

	uint32_t col_info = index + 31;
	if (FB_TYPE == 1) {
		FB_RED_FIELDPOS = multiboot_struct[col_info];
                FB_RED_MASKSIZE = multiboot_struct[col_info+1];	
                FB_GREEN_FIELDPOS = multiboot_struct[col_info+2]; 
                FB_GREEN_MASKSIZE = multiboot_struct[col_info+3];
                FB_BLUE_FIELDPOS = multiboot_struct[col_info+4];
                FB_BLUE_MASKSIZE = multiboot_struct[col_info+5];
	};
}

void put_pixel (uint32_t x, uint32_t y, uint32_t r, uint32_t g, uint32_t b) {
	uint32_t location = y * FB_PITCH + x * FB_BPP;
	uint32_t red_masked = r & ((1 << FB_RED_MASKSIZE) - 1);
	uint32_t green_masked = r & ((1 << FB_GREEN_MASKSIZE) - 1); 
	uint32_t blue_masked = r & ((1 << FB_BLUE_MASKSIZE) - 1);
	uint32_t color = (red_masked << FB_RED_FIELDPOS) | 
		(green_masked << FB_GREEN_FIELDPOS) |
		(blue_masked << FB_BLUE_FIELDPOS);

	FB_ADDR[location] = color;
}


