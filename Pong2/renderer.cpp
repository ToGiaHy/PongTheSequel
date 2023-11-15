
internal void clear_screen(unsigned int color) {
	unsigned int* pixel = (unsigned int*)render_background.memory;
	for (int y = 0; y < render_background.height; y++) {
		for (int x = 0; x < render_background.width; x++) {
			//change background colors
			*pixel++ = color;
		}
	}
}

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
