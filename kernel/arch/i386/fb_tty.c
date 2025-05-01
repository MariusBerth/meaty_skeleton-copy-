#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define FONT8x16_IMPLEMENTATION
#include <kernel/font8x16.h>

#include <kernel/tty.h>
#include <stdio.h>

static char* FB_ADDR;	//Voir pour rendre ça compatible avec le
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


struct color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct color FB_RED = { 255, 0, 0 };
struct color FB_GREEN = { 0, 255, 0 };
struct color FB_BLUE = { 0, 0, 255 };
struct color FB_WHITE = { 255, 255, 255 };
struct color FB_BLACK = { 0, 0, 0 };

uint32_t get_color ( struct color color ) {
	uint32_t red_masked = color.r & ((1 << FB_RED_MASKSIZE) - 1);
	uint32_t green_masked = color.g & ((1 << FB_GREEN_MASKSIZE) - 1); 
	uint32_t blue_masked = color.b & ((1 << FB_BLUE_MASKSIZE) - 1);
	return (red_masked << FB_RED_FIELDPOS) | 
		(green_masked << FB_GREEN_FIELDPOS) |
		(blue_masked << FB_BLUE_FIELDPOS);
}


static uint32_t fb_term_row;
static uint32_t FB_MAX_ROW;
static uint32_t fb_term_column;
static uint32_t FB_MAX_COLUMN;
static struct color fb_font_color;
static struct color fb_bg_color;


void read_multiboot_struct (char* multiboot_struct) {
	//impossible de faire de l'arithmétique de pointeurs avec void*,
	//utiliser char* à la place et faire du typecast pour récup les bonnes
	//valeurs. Normalement c'est bon dans cette fontion
	const uint32_t struct_size = *(uint32_t*)multiboot_struct;
	uint32_t index = 8;
	
	while ( index < struct_size && *(uint32_t*) (multiboot_struct + index) != 8 ) {
		index = index + *(uint32_t*) (multiboot_struct + index + 4);
		index = (uint32_t) (index + (8 - 1)) & -8;
	};

	if ( index >= struct_size ) {
		terminal_initialize ();
		printf("Couldn't find any info relative to the framebuffer in the\
				multiboot structure,\n now hanging...");
		while (true) ;};

	FB_ADDR = *(char**) (multiboot_struct + index + 8);
	FB_PITCH = *(uint32_t*) (multiboot_struct + index + 16);
       	FB_WIDTH = *(uint32_t*) (multiboot_struct + index + 20);
	FB_MAX_COLUMN = FB_WIDTH >> 3;
	FB_HEIGHT = *(uint32_t*) (multiboot_struct + index + 24);
	FB_MAX_ROW = FB_HEIGHT >> 4;
	FB_BPP = multiboot_struct[index + 28] >> 3;
	FB_TYPE = multiboot_struct[index + 29];

	uint32_t col_info = index + 32;		//La spec multiboot2 n'est pas réspéctée par GRUB, 
						//les infos de couleurs devraient être
						//à la position index+31 mais le champ réservé situé juste avant
						//est un uint16 dans l'implémentation de GRUB alors que c'est un
						//uint8 dans la spécification
	if (FB_TYPE == 1) {
		FB_RED_FIELDPOS = *(uint8_t*) (multiboot_struct + col_info);
                FB_RED_MASKSIZE = *(uint8_t*) (multiboot_struct + col_info + 1);	
                FB_GREEN_FIELDPOS = multiboot_struct[col_info+2]; 
                FB_GREEN_MASKSIZE = multiboot_struct[col_info+3];
                FB_BLUE_FIELDPOS = multiboot_struct[col_info+4];
                FB_BLUE_MASKSIZE = multiboot_struct[col_info+5];
	};
}

void put_pixel (uint32_t x, uint32_t y, uint32_t truecolor) {
	uint32_t location = y * FB_PITCH + x * FB_BPP; 
	
	*(uint32_t*)(FB_ADDR + location) = truecolor;	//Tel qu'écrit, comme FB_ADDR est un pointeur vers uint32_t,
					//les offsets sont exprimés en long (donc un offset de 1 fait 4 octets)
					//Update: corrigé
}

void fillrect (uint32_t x, uint32_t y, const uint32_t w, const uint32_t h, 
		struct color color) {
	char* pos = FB_ADDR + (y * FB_PITCH + x * FB_BPP); 
	char* linebeg = pos;
	/*uint32_t red_masked = r & ((1 << FB_RED_MASKSIZE) - 1);	//bonne valeur 8
	uint32_t green_masked = g & ((1 << FB_GREEN_MASKSIZE) - 1); 
	uint32_t blue_masked = b & ((1 << FB_BLUE_MASKSIZE) - 1); 
	uint32_t color = (red_masked << FB_RED_FIELDPOS) |  	//bonne valeur 16
		(green_masked << FB_GREEN_FIELDPOS) |		//bonne valeur 8
		(blue_masked << FB_BLUE_FIELDPOS);		//bonne valeur 0*/
	uint32_t truecolor = get_color (color);

	uint32_t i;
	uint32_t j;
	for (i = 0; i < h && i + y < FB_HEIGHT; i++) {
		for (j = 0; j < w && j + x < FB_WIDTH; j++) {
			*(uint32_t*) pos = truecolor;	//mm commentaire que pour put_pixel
							//update: corrigé
			pos += FB_BPP;
		};
		linebeg += FB_PITCH;
		pos = linebeg;
	};
}	

void fb_terminal_setup (void) {
	fb_term_row = 0;
	fb_term_column = 0;
	fb_font_color = FB_WHITE;
	fb_bg_color = FB_BLACK;
}

void fb_putentryat (uint32_t x, uint32_t y, struct color color, char c) {
	/*char* pos = FB_ADDR + (y * FB_PITCH + x * FB_BPP);
	char* linebeg = pos;*/
	c = c & 0x7F; 		//Masquage car les caractères ascii sont codés sur 7 bits
	unsigned char* code = font8x16[(unsigned char) c];
	uint32_t truecolor = get_color (color);

	uint32_t i;
	uint32_t j;
	for (i = 0; i < 16; i++) {
		unsigned char charline = code[i];
		for (j = 0; j < 8; j++) {
			uint8_t mask = 0x80 >> j;
			if (charline & mask) {
				put_pixel ((uint32_t) x + j, (uint32_t) y + i, truecolor);
			};
			//pos += FB_BPP;
		};
		//linebeg += FB_PITCH
	};
}

void fb_scroll (void) {
	uint32_t x;
	uint32_t y;
	uint32_t pos = 0;
	uint32_t linebeg = 0;
	uint32_t offset = 16 * FB_PITCH;

	for (x = 0; x < FB_HEIGHT - 16; x++) {
		for (y = 0; y < FB_WIDTH; y++) {
			*(uint32_t*) (FB_ADDR + pos) = 
				*(uint32_t*) (FB_ADDR + pos + offset);
			pos += FB_BPP;
		};
		linebeg += FB_PITCH;
		pos = linebeg;
	};

	fillrect (0, FB_HEIGHT-16, FB_WIDTH, 16, fb_bg_color);
}

void fb_putchar (char c) {
	if (c != '\n') {
		fb_putentryat (fb_term_column << 3, fb_term_row << 4, fb_font_color,
				c);
	};
	if (++fb_term_column == FB_MAX_COLUMN || c == '\n') {
		fb_term_column = 0;
		if (++fb_term_row == FB_MAX_ROW) {
			fb_scroll ();
			--fb_term_row;
		};
	};
}

void fb_write (char* str, size_t size) {
	for (size_t i = 0; i < size; i++) {
	       fb_putchar (str[i]);
	};
}

void fb_writeat (char* str, size_t size, uint32_t x, uint32_t y,
	       	struct color color) {
	for (uint32_t i = 0; i < size; i++) {
		if (x + 8 >= FB_WIDTH) {
			x = 0;
			if (y + 16 >= FB_HEIGHT) {
				y = 0;
			};
		};
		fb_putentryat (x, y, color, str[i]);
		x += 8;
	};
}

			       


void fb_writestring (char* str) {
	unsigned char data = *str;

	while (data != 0x00) {
		fb_putchar (data);
		str ++;
		data = *str;
	};
}


void framebuffer_initialize (char* multiboot_struct) {
	read_multiboot_struct (multiboot_struct);
	fillrect (0, 0, FB_WIDTH, FB_HEIGHT, FB_BLACK);
}
