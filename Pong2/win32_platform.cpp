#include "utilities.cpp"
#include <windows.h>

global_variable bool running = true;

struct Render_background {
	void* memory;
	int width;
	int height;
	BITMAPINFO bitmap_info;
};

global_variable Render_background render_background;


#include "controls.cpp"
#include "renderer.cpp"

WNDPROC Wndproc;

LRESULT CALLBACK windowCallBack(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY: {
		running = false;
	} break;

	case WM_SIZE: {
		RECT rect;
		GetClientRect(hwnd, &rect);
		render_background.width = rect.right - rect.left;
		render_background.height = rect.bottom - rect.top;

		int buffer_size = render_background.width * render_background.height * sizeof(unsigned int);

		if (render_background.memory) VirtualFree(render_background.memory, 0, MEM_RELEASE);
		render_background.memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		render_background.bitmap_info.bmiHeader.biSize = sizeof(render_background.bitmap_info.bmiHeader);
		render_background.bitmap_info.bmiHeader.biWidth = render_background.width;
		render_background.bitmap_info.bmiHeader.biHeight = render_background.height;
		render_background.bitmap_info.bmiHeader.biPlanes = 1;
		render_background.bitmap_info.bmiHeader.biBitCount = 32;
		render_background.bitmap_info.bmiHeader.biCompression = BI_RGB;
	}
	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
	return result;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	WNDCLASS windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = "Game Window Class";
	windowClass.lpfnWndProc = windowCallBack;

	RegisterClass(&windowClass);

	HWND window = CreateWindow(windowClass.lpszClassName, "Table Tennis", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
	{
		//Fullscreen
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	HDC hdc = GetDC(window);

	Input input = {};

	float delta_time = 0.016666f;
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);

	float performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (float)perf.QuadPart;
	}

	while (running) {
		MSG message;

		for (int i = 0; i < BUTTON_COUNT; i++) {
			input.buttons[i].changed = false;
		}
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			switch (message.message) {
			case WM_KEYUP:
			case WM_KEYDOWN: {
				u32 vk_code = (u32)message.wParam;
				bool is_down = ((message.lParam & (1 << 31)) == 0);
#define process_button(b, vk)\
case vk: {\
input.buttons[b].changed = is_down != input.buttons[b].is_down;\
input.buttons[b].is_down = is_down;\
}break;

				switch (vk_code) {
					process_button(BUTTON_UP, VK_UP);
					process_button(BUTTON_DOWN, VK_DOWN);
					process_button(BUTTON_W, 'W');
					process_button(BUTTON_S, 'S');
					process_button(BUTTON_A, 'A');
					process_button(BUTTON_D, 'D');
					process_button(BUTTON_ENTER, VK_RETURN);
				}
			}break;
			default: {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			}
		}

		simulate_game(&input, delta_time);

		StretchDIBits(hdc, 0, 0, render_background.width, render_background.height, 0, 0, render_background.width, render_background.height, render_background.memory, &render_background.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		frame_begin_time = frame_end_time;
	}


};
