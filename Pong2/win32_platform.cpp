
#include "utilities.cpp"
#include <windows.h>
#include <MMSystem.h>
#include <thread>
#include <stdio.h>

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
#include "game.cpp";

WNDPROC Wndproc;
internal void PlayMusic() {
	PlaySound(TEXT("MenuCyberpunk.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}
internal void PlayMusic2() {
	PlaySound(TEXT("FirstPhase.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}
internal void PlayMusic3() {
	PlaySound(TEXT("NewMeAndNewYou.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}internal void PlayMusic4() {
	PlaySound(TEXT("Victory.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}
internal void PlayMusic5() {
	PlaySound(TEXT("Defeat.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}
internal void PlayMusic6() {
	PlaySound(TEXT("SecondPhase.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

internal void PlayMusic7() {
	PlaySound(TEXT("LookThroughMyEyes.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

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
	std::thread musicThread(PlayMusic); // Start a separate thread for playing music
	musicThread.detach(); // Detach the thread (let it run independently)
	std::thread musicThread2(PlayMusic2); // Start a separate thread for playing music
	musicThread2.detach(); // Detach the thread (let it run independently)
	std::thread musicThread3(PlayMusic3); // Start a separate thread for playing music
	musicThread3.detach(); // Detach the thread (let it run independently)
	std::thread musicThread4(PlayMusic4); // Start a separate thread for playing music
	musicThread4.detach(); // Detach the thread (let it run independently)
	std::thread musicThread5(PlayMusic5); // Start a separate thread for playing music
	musicThread5.detach(); // Detach the thread (let it run independently)
	std::thread musicThread6(PlayMusic6); // Start a separate thread for playing music
	musicThread6.detach(); // Detach the thread (let it run independently)
	std::thread musicThread7(PlayMusic7); // Start a separate thread for playing music
	musicThread7.detach(); // Detach the thread (let it run independently)
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
					process_button(BUTTON_Q, 'Q');
					process_button(BUTTON_E, 'E');
					process_button(BUTTON_R, 'R');
					process_button(BUTTON_T, 'T');
					process_button(BUTTON_Y, 'Y');
					process_button(BUTTON_U, 'U');
					process_button(BUTTON_I, 'I');
					process_button(BUTTON_P, 'P');
					process_button(BUTTON_F, 'F');
					process_button(BUTTON_G, 'G');
					process_button(BUTTON_H, 'H');
					process_button(BUTTON_J, 'J');
					process_button(BUTTON_L, 'L');
					process_button(BUTTON_Z, 'Z');
					process_button(BUTTON_X, 'X');
					process_button(BUTTON_C, 'C');
					process_button(BUTTON_V, 'V');
					process_button(BUTTON_B, 'B');
					process_button(BUTTON_M, 'M');
					process_button(BUTTON_N, 'N');
					process_button(BUTTON_LEFT, VK_LEFT);
					process_button(BUTTON_RIGHT, VK_RIGHT);
					process_button(BUTTON_ENTER, VK_RETURN);
					process_button(BUTTON_SHIFT, VK_SHIFT);
					process_button(BUTTON_O, 'O');
					process_button(BUTTON_K, 'K');
					process_button(BUTTON_BACKSPACE, VK_BACK);
					process_button(BUTTON_SPACE, VK_SPACE);
					process_button(BUTTON_ESCAPE, VK_ESCAPE);
				}
			}break;
			default: {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			}
		}
		//winPlayer(&input, delta_time);
		//testQuestion(&input, 1, delta_time);
		//testQuestionMultiplayer(&input, 1, 1, delta_time);
		//pickName(&input, delta_time,0);
		//deleteProgress(&input, delta_time);
		//leaderboards(&input, delta_time);
		//tutorialSingle(&input, delta_time);
		simulate_game(&input, delta_time);
		if (quit) {
			running = false;
			break;
		}
		if (playMusic && !musicThread.joinable()) {
			musicThread = std::thread(PlayMusic);
		}
		else if (!playMusic && musicThread.joinable()) {
			PlaySound(NULL, NULL, 0); // Stop the sound
			musicThread.join(); // Wait for the music thread to finish
		}
		if (playMusic1 && !musicThread2.joinable()) {
			musicThread2 = std::thread(PlayMusic2);
		}
		else if (!playMusic1 && musicThread2.joinable()) {
			PlaySound(NULL, NULL, 0); // Stop the sound
			musicThread2.join(); // Wait for the music thread to finish
		}
		if (playMusic2 && !musicThread3.joinable()) {
			musicThread3 = std::thread(PlayMusic3);
		}
		else if (!playMusic2 && musicThread3.joinable()) {
			PlaySound(NULL, NULL, 0); // Stop the sound
			musicThread3.join(); // Wait for the music thread to finish
		}
		if (playMusic3 && !musicThread4.joinable()) {
			musicThread4 = std::thread(PlayMusic4);
		}
		else if (!playMusic3 && musicThread4.joinable()) {
			PlaySound(NULL, NULL, 0); // Stop the sound
			musicThread4.join(); // Wait for the music thread to finish
		}
		if (playMusic4 && !musicThread5.joinable()) {
			musicThread5 = std::thread(PlayMusic5);
		}
		else if (!playMusic4 && musicThread5.joinable()) {
			PlaySound(NULL, NULL, 0); // Stop the sound
			musicThread5.join(); // Wait for the music thread to finish
		}
		if (playMusic5 && !musicThread6.joinable()) {
			musicThread6 = std::thread(PlayMusic6);
		}
		else if (!playMusic5 && musicThread6.joinable()) {
			PlaySound(NULL, NULL, 0); // Stop the sound
			musicThread6.join(); // Wait for the music thread to finish
		}
		if (playMusic6 && !musicThread7.joinable()) {
			musicThread7 = std::thread(PlayMusic7);
		}
		else if (!playMusic6 && musicThread7.joinable()) {
			PlaySound(NULL, NULL, 0); // Stop the sound
			musicThread7.join(); // Wait for the music thread to finish
		}
		StretchDIBits(hdc, 0, 0, render_background.width, render_background.height, 0, 0, render_background.width, render_background.height, render_background.memory, &render_background.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		frame_begin_time = frame_end_time;
	}


};
