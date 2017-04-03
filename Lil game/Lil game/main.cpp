#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include "dxerr.h"
#include <DirectXMath.h>
#include <cstdio>
#include <iostream>
#include "Game.h"
#include "Globals.h"

using namespace DirectX;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"DX11_3D_PROJECT";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"DX11_3D_PROJECT",
		L"Dunkar Som Munkar",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	return handle;
}

HWND wndHandle;

long long time_ms() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	wndHandle = InitWindow(hInstance);


	if (wndHandle) {
	

		Game *game = new Game(wndHandle, WIDTH, HEIGHT);

		ShowWindow(wndHandle, nCmdShow);

		long long start = time_ms();
		long long prev = start;

		bool quit = false;
		while (!quit) {
			long long newtime = time_ms();
			long long elapsed = newtime - prev;

			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT)
					quit = true;

				if (msg.message == WM_KEYUP) {
					int wk = (int)msg.wParam;

					if (wk == VK_ESCAPE) quit = true;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			game->update((elapsed) / 1000.f);
			game->render();


			prev = newtime;
		}
		delete game;
		DestroyWindow(wndHandle);
	}

	return (int)msg.wParam;
}
