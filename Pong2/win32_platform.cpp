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
	
};
