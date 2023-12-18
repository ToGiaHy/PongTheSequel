
//clear screen function
internal void clear_screen(unsigned int color) {
	unsigned int* pixel = (unsigned int*)render_background.memory;
	for (int y = 0; y < render_background.height; y++) {
		for (int x = 0; x < render_background.width; x++) {
			//change background colors
			*pixel++ = color;
		}
	}
}

//draw rectangle in pixels in the main UI
internal void draw_rect_in_pixels(int x0, int y0, int x1, int y1, u32 color) {

	x0 = clamp(0, x0, render_background.width);
	x1 = clamp(0, x1, render_background.width);
	y0 = clamp(0, y0, render_background.height);
	y1 = clamp(0, y1, render_background.height);

	for (int y = y0; y < y1; y++) {
		u32* pixel = (unsigned int*)render_background.memory + x0 + y * render_background.width;
		for (int x = x0; x < x1; x++) {
			//change background colors
			*pixel++ = color;
		}
	}
}
global_variable float render_scale = 0.01f;

//draw the arena borders around the game
internal void
draw_arena_borders(float arena_x, float arena_y, u32 color) {
	arena_x *= render_background.height * render_scale;
	arena_y *= render_background.height * render_scale;

	int x0 = (int)((float)render_background.width * .5f - arena_x);
	int x1 = (int)((float)render_background.width * .5f + arena_x);
	int y0 = (int)((float)render_background.height * .5f - arena_y);
	int y1 = (int)((float)render_background.height * .5f + arena_y);

	draw_rect_in_pixels(0, 0, render_background.width, y0, color);
	draw_rect_in_pixels(0, y1, x1, render_background.height, color);
	draw_rect_in_pixels(0, y0, x0, y1, color);
	draw_rect_in_pixels(x1, y0, render_background.width, render_background.height, color);
}

//draw a rectangle with an increase in size
internal void draw_rect(float x, float y, float half_size_x, float half_size_y, u32 color) {
	x *= render_background.height * render_scale;
	y *= render_background.height * render_scale;
	half_size_x *= render_background.height * render_scale;
	half_size_y *= render_background.height * render_scale;

	x += render_background.width / 2.f;
	y += render_background.height / 2.f;

	// Change to pixels
	int x0 = x - half_size_x;
	int x1 = x + half_size_x;
	int y0 = y - half_size_y;
	int y1 = y + half_size_y;

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}

//uppercase text ascii art
const char* letters[][7] = {
	" 00",
	"0  0",
	"0  0",
	"0000",
	"0  0",
	"0  0",
	"0  0",

	"000",
	"0  0",
	"0  0",
	"000",
	"0  0",
	"0  0",
	"000",

	" 000",
	"0",
	"0",
	"0",
	"0",
	"0",
	" 000",

	"000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"000",

	"0000",
	"0",
	"0",
	"000",
	"0",
	"0",
	"0000",

	"0000",
	"0",
	"0",
	"000",
	"0",
	"0",
	"0",

	" 000",
	"0",
	"0",
	"0 00",
	"0  0",
	"0  0",
	" 000",

	"0  0",
	"0  0",
	"0  0",
	"0000",
	"0  0",
	"0  0",
	"0  0",

	"000",
	" 0 ",
	" 0 ",
	" 0 ",
	" 0 ",
	" 0 ",
	"000",

	" 000",
	"   0",
	"   0",
	"   0",
	"0  0",
	"0  0",
	" 000",

	"0  0",
	"0  0",
	"0 0",
	"00",
	"0 0",
	"0  0",
	"0  0",

	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0000",

	"00 00",
	"0 0 0",
	"0 0 0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",

	"00  0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0  00",

	"0000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0000",

	"0000",
	"0  0",
	"0  0",
	"0000",
	"0",
	"0",
	"0",

	" 000 ",
	"0   0",
	"0   0",
	"0   0",
	"0 0 0",
	"0  0 ",
	" 00 0",

	"000",
	"0  0",
	"0  0",
	"000",
	"0  0",
	"0  0",
	"0  0",

	" 000",
	"0",
	"0 ",
	" 00",
	"   0",
	"   0",
	"000 ",

	"000",
	" 0",
	" 0",
	" 0",
	" 0",
	" 0",
	" 0",

	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	" 00",

	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	" 0 0",
	"  0",

	"0   0 ",
	"0   0",
	"0   0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	" 0 0 ",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	" 0 0",
	"0   0",
	"0   0",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	"  0",
	"  0",
	"  0",

	"0000",
	"   0",
	"  0",
	" 0",
	"0",
	"0",
	"0000",

	"",
	"",
	"",
	"",
	"",
	"",
	"0",

	"   0",
	"  0",
	"  0",
	" 0",
	" 0",
	"0",
	"0",

	" 000 ",
	"0   0",
	"   0 ",
	"  0  ",
	" 0   ",
	"     ",
	" 0   ",

	" 0 ",
	" 0 ",
	" 0 ",
	" 0 ",
	" 0 ",
	"   ",
	" 0 ",

	" 000 ",
	" 0 ",
	" 0 ",
	" 0 ",
	" 0 ",
	" 0 ",
	" 000 ",

	"000",
	"  0",
	"  0",
	"  0",
	"  0",
	"  0",
	"000",

  "   0",
  "  000",
  " 0 0 0",
  "0  0  0",
  "   0",
  "   0",
  "   0",
  "   0",

  "   0",
  "   0",
  "   0",
  "0  0  0",
  " 0 0 0",
  "  000",
  "   0 ",

};


internal void draw_text(const char* text, float x, float y, float size, u32 color) {
	float half_size = size * .5f;
	float original_y = y;

	while (*text) {
		if (*text != 32) {
			const char** letter;
			if (*text == 47) letter = letters[27]; // '/' character
			else if (*text == 46) letter = letters[26]; // '.' character
			else if (*text == '?') letter = letters[28]; // '?' character
			else if (*text == '!') letter = letters[29];
			else if (*text == '[') letter = letters[30];
			else if (*text == ']') letter = letters[31];
			else if (*text == '>') letter = letters[32];
			else if (*text == '<') letter = letters[33];
			else letter = letters[*text - 'A'];
			float original_x = x;

			for (int i = 0; i < 7; i++) {
				const char* row = letter[i];
				while (*row) {
					if (*row == '0') {
						draw_rect(x, y, half_size, half_size, color);
					}
					x += size;
					row++;
				}
				y -= size;
				x = original_x;
			}
		}
		text++;
		x += size * 6.f;
		y = original_y;
	}
}

//draw number method
internal void
draw_number(int number, float x, float y, float size, u32 color) {
	float half_size = size * .5f;

	bool drew_number = false;
	while (number || !drew_number) {
		drew_number = true;

		int digit = number % 10;
		number = number / 10;

		switch (digit) {
		case 0: {
			draw_rect(x - size, y, half_size, 2.5f * size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x, y + size * 2.f, half_size, half_size, color);
			draw_rect(x, y - size * 2.f, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 1: {
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			x -= size * 2.f;
		} break;

		case 2: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x, y, 1.5f * size, half_size, color);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x + size, y + size, half_size, half_size, color);
			draw_rect(x - size, y - size, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 3: {
			draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
			draw_rect(x - half_size, y, size, half_size, color);
			draw_rect(x - half_size, y - size * 2.f, size, half_size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			x -= size * 4.f;
		} break;

		case 4: {
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x - size, y + size, half_size, 1.5f * size, color);
			draw_rect(x, y, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 5: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x, y, 1.5f * size, half_size, color);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x - size, y + size, half_size, half_size, color);
			draw_rect(x + size, y - size, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 6: {
			draw_rect(x + half_size, y + size * 2.f, size, half_size, color);
			draw_rect(x + half_size, y, size, half_size, color);
			draw_rect(x + half_size, y - size * 2.f, size, half_size, color);
			draw_rect(x - size, y, half_size, 2.5f * size, color);
			draw_rect(x + size, y - size, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 7: {
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
			x -= size * 4.f;
		} break;

		case 8: {
			draw_rect(x - size, y, half_size, 2.5f * size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x, y + size * 2.f, half_size, half_size, color);
			draw_rect(x, y - size * 2.f, half_size, half_size, color);
			draw_rect(x, y, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 9: {
			draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
			draw_rect(x - half_size, y, size, half_size, color);
			draw_rect(x - half_size, y - size * 2.f, size, half_size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x - size, y + size, half_size, half_size, color);
			x -= size * 4.f;
		} break;
		}

	}
}

//lowercase ascii art
const char* undercaseletters[][7] = {
	//undercase
	"    ", // 'a'
	"    ",
	"0000",
	"   0",
	"0000",
	"0  0",
	"0000",

	"    ", // 'b'
	"    ",
	"0   ",
	"0   ",
	"0000",
	"0  0",
	"0000",

	"    ", // 'c'
	"    ",
	" 000",
	"0   ",
	"0   ",
	"0   ",
	" 000",

	"    ", // 'd'
	"    ",
	"   0",
	"   0",
	"0000",
	"0  0",
	"0000",

	"    ", // 'e'
	"    ",
	"0000",
	"0  0",
	"0000",
	"0",
	"0000",

	"    ", // 'f'
	"    ",
	" 00 ",
	"0  ",
	"000 ",
	"0",
	"0",

	"    ", // 'g'
	"    ",
	" 000",
	"0   ",
	"0 00",
	"0  0",
	" 000",

	"    ", // 'h'
	"    ",
	"0   ",
	"0   ",
	"0000",
	"0  0",
	"0  0",

	"  ", // 'i'
	"  ",
	"0 ",
	"  ",
	"0 ",
	"0 ",
	"00",

	"   ", // 'j'
	"   ",
	" 0 ",
	"  ",
	" 0",
	" 0",
	"00",

	"    ", // 'k'
	"    ",
	"0  0",
	"0 0 ",
	"00  ",
	"0 0 ",
	"0  0",

	"    ", // 'l'
	"    ",
	"0   ",
	"0   ",
	"0   ",
	"0   ",
	"0000",

	"     ", // 'm'
	"     ",
	"0 0 0",
	"00 00",
	"0 0 0",
	"0   0",
	"0   0",

	"    ", // 'n'
	"    ",
	"00  ",
	"0 0 ",
	"0 0 ",
	"0 0 ",
	"0 0 ",

	"    ", // 'o'
	"    ",
	" 00 ",
	"0  0",
	"0  0",
	"0  0",
	" 00 ",

	"    ", // 'p'
	"    ",
	"000 ",
	"0  0",
	"000 ",
	"0   ",
	"0   ",

	"    ", // 'q'
	"    ",
	" 00 ",
	"0  0",
	"0  0",
	"0 00",
	" 00 ",

	"    ", // 'r'
	"    ",
	"000 ",
	"0  0",
	"0   ",
	"0   ",
	"0   ",

	"    ", // 's'
	"    ",
	" 000",
	"0   ",
	" 00 ",
	"   0",
	"000 ",

	"    ", // 't'
	"    ",
	"0000",
	" 0  ",
	" 0  ",
	" 0  ",
	" 0  ",

	"    ", // 'u'
	"    ",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	" 00 ",

	"    ", // 'v'
	"    ",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	" 00 ",

	"     ", // 'w'
	"     ",
	"0   0",
	"0   0",
	"0 0 0",
	"00 00",
	"0   0",

	"    ", // 'x'
	"    ",
	"0  0",
	"0  0",
	" 00 ",
	"0  0",
	"0  0",

	"    ", // 'y'
	"    ",
	"0  0",
	"0  0",
	" 00 ",
	" 0  ",
	" 0  ",

	"    ", // 'z'
	"    ",
	"0000",
	"   0",
	"  0 ",
	" 0  ",
	"0000",
};

//draw ascii art
void render_ascii_art(const char* ascii_art[], int width, int height, float x, float y, float size, u32 color, u32 secondColor, u32 thirdColor) {
	float half_size = size * .5f;
	float original_y = y;

	for (int i = 0; i < height; ++i) {
		const char* row = ascii_art[i];
		float original_x = x;

		while (*row) {
			if (*row == '0') {
				draw_rect(x, y, half_size, half_size, color);
			}
			if (*row == '1') {
				draw_rect(x, y, half_size, half_size, secondColor);
			}
			if (*row == '2') {
				draw_rect(x, y, half_size, half_size, thirdColor);
			}

			x += size;
			row++;
		}
		y -= size;
		x = original_x;
	}
}

//draw lowercase words
void draw_lowercase_letter(const char* text, float x, float y, float size, u32 color) {
	float half_size = size * .5f;
	float original_y = y;

	while (*text) {
		if (*text != 32 && *text >= 'a' && *text <= 'z') {
			const char** letter = undercaseletters[*text - 'a'];
			float original_x = x;

			for (int i = 0; i < 7; i++) {
				const char* row = letter[i];
				while (*row) {
					if (*row == '0') {
						draw_rect(x, y, half_size, half_size, color);
					}
					x += size;
					row++;
				}
				y -= size;
				x = original_x;
			}
		}
		text++;
		x += size * 6.f; // Adjust spacing between characters
		y = original_y;
	}
}
