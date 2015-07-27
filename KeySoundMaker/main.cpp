# include <stdio.h>
# include <conio.h>
# include <iostream>
# include <list>
# include <algorithm>
# include <windows.h>
# include <winuser.h>
# include <mmsystem.h>

# include "soundEngine.h"

//#define OUTFILE_NAME	"Logs\\WinKey.log"	/* Output file */
#define CLASSNAME	"winkey"
#define WINDOWTITLE	"KeySoundMaker"

char	windir[MAX_PATH + 1];
HHOOK	kbdhook;	/* Keyboard hook handle */
bool	running;	/* Used in main loop */
std::list<DWORD> pressedKeys;
HANDLE bgThreadHandle;
SoundEngine sound("..\\CMStormTKBlue");


bool keyReleased(const DWORD value) {
	if (GetAsyncKeyState(value) & 0x8000)
		return false;
	return true;
}

// called by Windows
__declspec(dllexport) LRESULT CALLBACK handlekeys(int code, WPARAM wp, LPARAM lp)
{
	if (code == HC_ACTION) {
		KBDLLHOOKSTRUCT st_hook = *((KBDLLHOOKSTRUCT*)lp);

		if (wp == WM_SYSKEYDOWN || wp == WM_KEYDOWN) {
			bool found = (std::find(pressedKeys.begin(), pressedKeys.end(), st_hook.vkCode) != pressedKeys.end());
			if (!found) {
				sound.playDownSound(st_hook.vkCode);
				pressedKeys.push_back(st_hook.vkCode);
			}
		} else if (wp == WM_SYSKEYUP || wp == WM_KEYUP) {			
			sound.playUpSound(st_hook.vkCode);
			pressedKeys.remove(st_hook.vkCode);
		}
	}
	return CallNextHookEx(kbdhook, code, wp, lp);
}


LRESULT CALLBACK windowprocedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_CLOSE: case WM_DESTROY:
		running = false;
		break;
	default:
		/* Call default message handler */
		return DefWindowProc(hwnd, msg, wp, lp);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE thisinstance, HINSTANCE previnstance,
	LPSTR cmdline, int ncmdshow)
{
	/*
	* Set up window
	*/
	HWND		hwnd;
	HWND		fgwindow = GetForegroundWindow(); /* Current foreground window */
	MSG		msg;
	WNDCLASSEX	windowclass;
	HINSTANCE	modulehandle;

	windowclass.hInstance = thisinstance;
	windowclass.lpszClassName = CLASSNAME;
	windowclass.lpfnWndProc = windowprocedure;
	windowclass.style = CS_DBLCLKS;
	windowclass.cbSize = sizeof(WNDCLASSEX);
	windowclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowclass.lpszMenuName = NULL;
	windowclass.cbClsExtra = 0;
	windowclass.cbWndExtra = 0;
	windowclass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	if (!(RegisterClassEx(&windowclass)))
		return 1;

	hwnd = CreateWindowEx(NULL, CLASSNAME, WINDOWTITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, HWND_DESKTOP, NULL,
		thisinstance, NULL);
	if (!(hwnd))
		return 1;

	/*
	* Make the window invisible
	*/

	ShowWindow(hwnd, SW_SHOW);
	//ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);
	SetForegroundWindow(fgwindow); /* Give focus to the previous fg window */

	/*
	* Hook keyboard input so we get it too
	*/
	modulehandle = GetModuleHandle(NULL);
	kbdhook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)handlekeys, modulehandle, NULL);

	running = true;

	GetWindowsDirectory((LPSTR)windir, MAX_PATH);



	/*
	* Main loopc
	*/
	while (running) {
		/*
		* Get messages, dispatch to window procedure
		*/
		if (!GetMessage(&msg, NULL, 0, 0))
			running = false; /*
							 * This is not a "return" or
							 * "break" so the rest of the loop is
							 * done. This way, we never miss keys
							 * when destroyed but we still exit.
							 */
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}