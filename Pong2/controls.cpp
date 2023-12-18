struct Button_state {
	bool is_down;
	bool changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_W,
	BUTTON_S,
	BUTTON_A,
	BUTTON_D,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_ENTER,
	BUTTON_I,
	BUTTON_K,
	BUTTON_O,
	BUTTON_ESCAPE,
	BUTTON_COUNT,
};

struct Input {
	Button_state buttons[BUTTON_COUNT];
};