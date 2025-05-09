#include <stdint.h>
#include <kernel/fb_tty.h>
#include <drivers/keyboard.h>

uint32_t game_state;
uint32_t base_x = 640;
uint32_t base_y = 360; 	//valeurs de base si problème, ne devraient pas être
		       	//utilisées
const uint32_t icon_size = 85;	// moitié de la taille des icones de jeux (x et o) en px
const uint32_t gridsize = 600;

struct color bg_color;
struct color font_color;

void draw_grid (uint32_t cx,uint32_t  cy, uint32_t size) {
	// Dessine une grille 3x3 centrée en cx, cy, inscrite dans un
	// carré de taille sizexsize px
	fillrect (cx - size / 6, cy - (size >> 1), 1, size, font_color);
	fillrect (cx + size / 6, cy - (size >> 1), 1, size, font_color);
	fillrect (cx - (size >> 1), cy - size / 6, size, 1, font_color);
	fillrect (cx - (size >> 1), cy + size / 6, size, 1, font_color);
}

void draw_circle (uint32_t cx, uint32_t cy) {
	bresenham_jesken_circle (cx, cy, icon_size, font_color);
}

void draw_cross (uint32_t cx, uint32_t cy) {
	uint32_t truecolor = get_color (font_color);
	uint32_t x;
	uint32_t boty = cy + icon_size;
	uint32_t topy = cy - icon_size;
	
	for (x = cx - icon_size; x <= cx + icon_size; x++) {
		put_pixel (x, boty, truecolor);
		put_pixel (x, topy, truecolor);
		put_pixel (x, boty--, truecolor);
		put_pixel (x, topy++, truecolor);
	};
}

uint8_t box_num_from_input (uint8_t input) {
	if (0x10 <= input && input <= 0x12) {
		return (input - 0x10);}
	if (0x1E <= input && input <= 0x20) {
		return (input - 0x1E + 3);}
	if (0x2C <= input && input <= 0x2E) {
		return (input - 0x2C + 6);}
	return 0xFF;	//l'entrée n'est pas dans la plage de valeurs valides
			//à noter qu'il faut toujours vérifier que l'on ne 
			//joue pas sur une case déja remplie.
}

void player_turn (void) {
	static uint8_t current_turn_x;
	fillrect (0, 0, FB_WIDTH, 16, bg_color);
	if (current_turn_x) {
		fb_writeat("Tour du joueur x...", 0, 0, font_color);
	}
	else {
		fb_writeat("Tour du joueur o...", 0, 0, font_color);
	};
	uint8_t box_num = box_num_from_input (keyboard_read());
	if ((box_num == 0xFF) | (game_state & (1 << box_num)) |
		       	(game_state & (1 << (box_num + 16)))) {
		fb_writeat("Ce coup est invalide, veuillez en choisir un autre.",
				0, 16, font_color);
		box_num = box_num_from_input (keyboard_read());
		while ((box_num == 0xFF) | (game_state & (1 << box_num)) |
		       	(game_state & (1 << (box_num + 16)))) {
			box_num = box_num_from_input (keyboard_read());
		};
		fillrect (0, 16, FB_WIDTH, 16, bg_color);
	};

	uint32_t cx = base_x + ((gridsize / 3) * ((box_num % 3) - 1));
	uint32_t cy = base_y + ((gridsize / 3) * ((box_num / 3) - 1));

	if (current_turn_x) {
		draw_cross (cx, cy);
		game_state |= 0x01 << box_num;
	}
	else {
		draw_circle(cx, cy);
		game_state |= 0x01 << (box_num + 16);
	};

	current_turn_x = !current_turn_x;
}

//les lignes sont 0x07, 0x38, 0x01C0, 0x49, 0x92, 0x0124, 0x0111, 0x54
int8_t check_game_end (void) {
	int16_t xbox = (int16_t) game_state;
	int16_t obox = (int16_t) (game_state >> 16);

	if (!((~(xbox | (~ 0x07))) && (~(xbox | (~ 0x38))) && (~(xbox | (~ 0x01C0))) 
			&& (~(xbox | (~ 0x49))) && (~(xbox | (~ 0x92))) && (~(xbox | (~ 0x0124))) 
			&& (~(xbox | (~ 0x0111))) && (~(xbox | (~ 0x54))) )) {
		return 1;
	};
	if (!((~(obox | (~ 0x07))) && (~(obox | (~ 0x38))) && (~(obox | (~ 0x01C0))) 
			&& (~(obox | (~ 0x49))) && (~(obox | (~ 0x92))) && (~(obox | (~ 0x0124))) 
			&& (~(obox | (~ 0x0111))) && (~(obox | (~ 0x54))) )) {
		return 2;
	};
	if (!(~(xbox | obox | -(1 << 9)))) {
		return 4;
	};
	return 0;
}

	

void morpion (void) {
        bg_color = fb_get_bg_color();
        font_color = fb_get_font_color();


	game_state = 0;

	fillrect (0, 0, FB_WIDTH, FB_HEIGHT, bg_color);
	base_x = FB_WIDTH / 2;
	base_y = FB_HEIGHT / 2;
	draw_grid (base_x, base_y, gridsize);

	uint32_t helpsize = gridsize >> 2;	//paragraphe pour afficher l'aide de controle
	uint32_t hx = (base_x - 3* helpsize) - 4;
	uint32_t hy = base_y - 8;		//les décalages constants sont la
						//car les coordonées des glyphes 
						//ne sont pas données par le centre
	draw_grid (hx + 4 , hy + 8, helpsize);
	uint32_t off = helpsize / 3;
	fb_writeat ("q", hx - off, hy - off, font_color);
	fb_writeat ("w", hx, hy - off, font_color);
	fb_writeat ("e", hx + off, hy - off, font_color);
	fb_writeat ("a", hx - off, hy, font_color);
	fb_writeat ("s", hx, hy, font_color);
	fb_writeat ("d", hx + off, hy, font_color);
	fb_writeat ("z", hx - off, hy + off, font_color);
	fb_writeat ("x", hx, hy + off, font_color);
	fb_writeat ("c", hx + off, hy + off, font_color);

	while (! check_game_end()) {
		player_turn ();
	};
	fillrect (0, 0, FB_WIDTH, 16, bg_color);
	fb_writestring ("partie finie, ");
	int8_t winner = check_game_end ();
	if (winner & 1) {
		fb_writestring ("le.a joueur.euse x a gagne");}
	else if (winner & 2) {
		fb_writestring ("le.a joueur.euse o a gagne");}
	else fb_writestring ("c'est une egalite");
        fb_writestring ("\nappuyez sur une touche pour quitter\n");
        keyboard_read();
}
