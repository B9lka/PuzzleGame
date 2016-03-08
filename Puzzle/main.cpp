#include <windows.h>
#include <windowsx.h>
#include "ResourceManager.hpp"
#include "GDIWorker.hpp"
#include "Game.hpp"



// locking to 30 fps
const int MS_PER_FRAME = 1000 / 30;
const int MAX_WINDOW_WIDTH = 815;
const int MAX_WINDOW_HEIGHT = 635;

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

{
	GDIWorker::Init(); //Initialization gdi+
	MSG msg;
	HWND hwnd;
	RECT rect;
	Game* game = new Game();

	WNDCLASS wc;

	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0L;
	wc.cbWndExtra = 0L;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockBrush(BLACK_BRUSH);
	wc.lpszMenuName = NULL;//MAKEINTRESOURSE(IDR_MENU1);
	wc.lpszClassName = L"GameWindow";

	RegisterClass(&wc);
//--------------------------------
	GetClientRect(GetDesktopWindow(), &rect);

	hwnd = CreateWindow(
		L"GameWindow",
		L"Puzzle 14.88",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		(rect.right / 2) - 300,	20,	600, 600,
		HWND_DESKTOP,
		NULL,
		GetModuleHandle(0),
		game);

	GetClientRect(hwnd, &rect);
	
	ShowWindow(hwnd, nCmdShow);
//--------------------------------		
	
	game->Initialize();
	game->Render();

	while (GetMessage(&msg, NULL, 0, 0))
	{
		int timeStart = (int)GetTickCount64(); 

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
		while ((GetTickCount64() - timeStart) < MS_PER_FRAME)
		{
			int pause = (MS_PER_FRAME - (GetTickCount64() - timeStart));
			if (pause < 0)
				break;
			Sleep(pause);
		}
	}
	
	ResourceManager::GetInstance().DestroyTextures();
	GDIWorker::Destroy();
	return 0;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	Game* game;
	CREATESTRUCT *pCreate;
	
	switch (Msg)
	{
		case WM_CREATE:
		{
			pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			game = reinterpret_cast<Game*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)game);
			return 0;
		}
		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO info = (LPMINMAXINFO)lParam;
			info->ptMaxTrackSize.x = MAX_WINDOW_WIDTH;
			info->ptMaxTrackSize.y = MAX_WINDOW_HEIGHT;

			info->ptMinTrackSize.x = MAX_WINDOW_WIDTH;
			info->ptMinTrackSize.y = MAX_WINDOW_HEIGHT;

			return 0;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			PostQuitMessage(0);
		}
		case WM_PAINT:
		{
			LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
			game = reinterpret_cast<Game*>(ptr);
			
			Game::mmgdi->BeginScene();
			Game::mmgdi->EndScene();
	
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
			game = reinterpret_cast<Game*>(ptr);

			game->Click((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));

			game->UpdateEffects();
			game->Render();

			return 0;
		}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
};