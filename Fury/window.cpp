#include "immediateio.h"
#define WIN32_LEAN_AND_MEAN
#include "window.h"
#include <windows.h>
#include <windowsx.h>
#include <cassert>
#include <iostream>

	Window::Window()
	{
	}

	HWND Window::getHwnd()
	{
		return mHwnd;
	}

	bool Window::initialise(HINSTANCE hInstance)
	{
		const wchar_t* className = L"Fury";

		//Client size:
		mHeight = 600;
		mWidth = 800;

		WNDCLASSEX wc = 
		{
			sizeof(WNDCLASSEX),
			CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
			WndProc,
			0,
			0,
			hInstance,
			LoadIcon(NULL,IDI_APPLICATION),
			LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_WINDOW+1),
			NULL,
			className,
			LoadIcon(NULL, IDI_APPLICATION)
		};

		if(!RegisterClassEx(&wc))
			{
				Error("Could not register class.");
				return false;
			}

		int exParams = WS_EX_OVERLAPPEDWINDOW;
		int otParams =  WS_BORDER | WS_OVERLAPPEDWINDOW;

		int absoluteSizeX,absoluteSizeY;
		{
			RECT rect = {0,0,mWidth, mHeight};
			AdjustWindowRect(&rect, otParams, FALSE);
			absoluteSizeX = rect.right-rect.left;
			absoluteSizeY = rect.bottom-rect.top;
		}

		mHwnd = CreateWindowEx(exParams, className, L"Fury",
			otParams,
			CW_USEDEFAULT, CW_USEDEFAULT, absoluteSizeX, absoluteSizeY, NULL, NULL, hInstance, NULL);
		if(!mHwnd)
		{
			Error("Could not create window.");
			return false;
		}

		ShowWindow(mHwnd, SW_SHOWDEFAULT);

		mIsOpen  = true;

		mInput.initialise(mHwnd);

		return true;
	}

	void Window::shutdown()
	{
		mInput.shutdown();
	}

	bool Window::isOpen()
	{
		return mIsOpen;
	}

	void Window::close()
	{
		mIsOpen =false;
		DestroyWindow(mHwnd);
	}


	bool Window::pollEvent(Event* e)
	{
		MSG msg;
		if (PeekMessage(&msg, mHwnd, 0, 0, PM_REMOVE))
		{

			mTmpEvent.type = Event::Undefined; //To be altered in the WindProc

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_INPUT)
				mInput.frame(msg);

			if (mTmpEvent.type == Event::Undefined)
				return false; //WndProc no longer working. Program likely shutting down.

			*e = mTmpEvent;
			return true;
		}
		else
			return false;
	}


	LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
	switch (msg)
	{
		case WM_CLOSE:
		mTmpEvent.type = Event::Close;
		break;

		case WM_DESTROY:
		PostQuitMessage(0);

		 break;

		default:

		mTmpEvent.type = Event::Empty;

		return DefWindowProc(hwnd,msg,wparam,lparam);
	}

	return 0;
}

	Input& Window::getInput()
	{
		return mInput;
	}

Event Window::mTmpEvent;