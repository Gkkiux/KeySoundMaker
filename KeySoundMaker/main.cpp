# include <stdio.h>
# include <conio.h>
# include <iostream>
# include <list>
# include <vector>
# include <algorithm>
# include <windows.h>
# include <winuser.h>
# include <mmsystem.h>
# include <dirent.h>

# include "soundEngine.h"

#define CLASSNAME	"winkey"
#define WINDOWTITLE	"KeySoundMaker"
#define TRAY_FOLDER_ITEM 4000
#define TRAY_EXIT_ITEM 5000
#define WM_TRAYICON (WM_USER + 1)

char	windir[MAX_PATH + 1];
HHOOK	kbdhook;	/* Keyboard hook handle */
NOTIFYICONDATA trayIcon;
HWND hwnd;
HMENU trayMenu;
HMENU folderSelect;
bool	running;	/* Used in main loop */
std::list<DWORD> pressedKeys;
std::vector<char*> directories;
SoundEngine sound("CMStormTKBlue");

bool keyReleased(const DWORD value) {
	if (GetAsyncKeyState(value) & 0x8000)
		return false;
	return true;
}

void addFolders() {
	DIR *dir;
	struct dirent *ent;
	int i = 0;
	char *buf;
	if ((dir = opendir("./")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_DIR) {
				if (!strstr(ent->d_name, ".")) {

					AppendMenu(folderSelect, MF_POPUP, TRAY_FOLDER_ITEM + i, ent->d_name);
					buf = (char*)malloc(strlen(ent->d_name));
					strcpy(buf, ent->d_name);
					directories.push_back(buf);
					i++;
				}
			}
		}
		closedir(dir);
	}
}

void initNotifyIconData() {
	memset(&trayIcon, 0, sizeof(NOTIFYICONDATA));
	trayIcon.cbSize = sizeof(NOTIFYICONDATA);

	trayIcon.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	trayIcon.hWnd = hwnd;
	strcpy(trayIcon.szTip, "KeySoundMaker");
	trayIcon.uCallbackMessage = WM_TRAYICON;
	trayIcon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	trayIcon.uID = 1; 
	trayIcon.hIcon = (HICON)LoadImage(NULL, TEXT("mxblue256.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

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

LRESULT CALLBACK windowprocedure(HWND l_hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_CREATE:
		trayMenu = CreatePopupMenu();	
		folderSelect = CreatePopupMenu();
		addFolders();
		AppendMenu(trayMenu, MF_POPUP, (UINT_PTR)folderSelect, "Select Folder...");
		AppendMenu(trayMenu, MF_STRING, TRAY_EXIT_ITEM, "Exit");
		break;

	case WM_TRAYICON:
		//if (lp == WM_LBUTTONUP)
			//restore();
		if ((lp == WM_LBUTTONUP) || (lp == WM_RBUTTONUP)) {
			POINT curPoint;
			GetCursorPos(&curPoint);
			SetForegroundWindow(l_hwnd);

			UINT clicked = TrackPopupMenu(
				trayMenu,
				TPM_RETURNCMD | TPM_NONOTIFY,
				curPoint.x,
				curPoint.y,
				0, l_hwnd, NULL);

			if (clicked == TRAY_EXIT_ITEM) {
				running = false;
			}
			else if ((clicked - TRAY_FOLDER_ITEM < directories.size()) && clicked - TRAY_FOLDER_ITEM >= 0)
				sound = SoundEngine(directories[clicked - TRAY_FOLDER_ITEM]);

			
		};
		break;
	case WM_NCHITTEST:	{					//??????????????????????????
		UINT uHitTest = DefWindowProc(l_hwnd, WM_NCHITTEST, wp, lp);
		if (uHitTest == HTCLIENT)
			return HTCAPTION;
		else
			return uHitTest;
	}
	case WM_CLOSE: 
	case WM_DESTROY:
		running = false;
		break;
	default:
		/* Call default message handler */
		return DefWindowProc(l_hwnd, msg, wp, lp);
	}

	return DefWindowProc(l_hwnd, msg, wp, lp);
}

//int main(int argc, char *argv[]) {
//	MSG msg;
//	HINSTANCE modulehandle;
//	
//	hwnd = FindWindow("ConsoleWindowClass", NULL);
//	//ShowWindow(hwnd, 0);
//
//	memset(&trayIcon, 0, sizeof(NOTIFYICONDATA));
//
//	trayIcon.cbSize = sizeof(NOTIFYICONDATA);
//	trayIcon.hIcon = LoadIcon(NULL, IDI_WINLOGO);
//	trayIcon.hWnd = hwnd;
//	strcpy(trayIcon.szTip, "Press to open output console");
//	trayIcon.uCallbackMessage = WM_LBUTTONDOWN;
//	trayIcon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
//	trayIcon.uID = 1;
//	
//
//	Shell_NotifyIcon(NIM_ADD, &trayIcon);
//
//	modulehandle = GetModuleHandle(NULL);
//
//	kbdhook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)handlekeys, modulehandle, NULL);
//	
//	running = true;
//	
//	GetWindowsDirectory((LPSTR)windir, MAX_PATH);
//
//	while (running) {
//			/*
//			* Get messages, dispatch to window procedure
//			*/
//		if (!GetMessage(&msg, NULL, 0, 0))
//			running = false; /*
//								* This is not a "return" or
//								* "break" so the rest of the loop is
//								* done. This way, we never miss keys
//								* when destroyed but we still exit.
//								*/
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//}

int WINAPI WinMain(HINSTANCE thisinstance, HINSTANCE previnstance,
	LPSTR cmdline, int ncmdshow)
{
	/*
	* Set up window
	*/
	
	HWND		fgwindow = GetForegroundWindow(); /* Current foreground window */
	MSG			msg;
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
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, HWND_DESKTOP, NULL,
		thisinstance, NULL);
	if (!(hwnd))
		return 2;

	initNotifyIconData();

	/*
	* Make the window invisible
	*/

	Shell_NotifyIcon(NIM_ADD, &trayIcon);

	//ShowWindow(hwnd, SW_SHOW);
	ShowWindow(hwnd, SW_HIDE);
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

	Shell_NotifyIcon(NIM_DELETE, &trayIcon);

	return 0;
}