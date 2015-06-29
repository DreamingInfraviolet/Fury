#include "input.h"
#include "System.h"

Input::Input()
{
}

bool Input::initialise(HWND hwnd)
{
	mHwnd = hwnd;

	keys.resize(256, false);
	deltaMouseX = deltaMouseY = 0;

	RAWINPUTDEVICE rIDev[2];

	//Mouse
	rIDev[0].dwFlags = 0;
	rIDev[0].hwndTarget = 0;
	rIDev[0].usUsage = 0x02;
	rIDev[0].usUsagePage = 0x01;

	//keyboad
	rIDev[1].dwFlags = 0;
	rIDev[1].hwndTarget = 0;
	rIDev[1].usUsage = 0x06;
	rIDev[1].usUsagePage = 0x01;

	if (!RegisterRawInputDevices(rIDev, 2, sizeof(rIDev[0])))
		return false;

	return true;
}

void Input::shutdown() {}

void Input::frame(MSG& msg)
{
	static UINT dwSize;

	GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

	deltaMouseX = deltaMouseY = 0;
	RAWINPUT* buffer = (RAWINPUT*)malloc(dwSize);

	if (GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, buffer, &dwSize, sizeof(RAWINPUTHEADER)))
	{
		//if keyboard
		if (buffer->header.dwType == RIM_TYPEKEYBOARD)
		{
			//key down
			if (buffer->data.keyboard.Flags == RI_KEY_MAKE)
			{
				if (buffer->data.keyboard.VKey == Input::Code::Esc)
					PostQuitMessage(0);
				keys[buffer->data.keyboard.VKey] = true;
			}
			//key up
			else if
				(buffer->data.keyboard.Flags == RI_KEY_BREAK)
			{
				keys[buffer->data.keyboard.VKey] = false;
			}
		}
		//if mouse
		//Checks border stuff too
		else if (buffer->header.dwType == RIM_TYPEMOUSE)
		{
			buffer->data.mouse.ulButtons;

			deltaMouseX = buffer->data.mouse.lLastX;
			deltaMouseY = buffer->data.mouse.lLastY;

		}
	}

	free(buffer);
}

bool Input::onKeyDown(WPARAM key)
{
	return keys[key];
}
bool Input::onKeyUp(WPARAM key)
{
	return !keys[key];
}

math::vec2 Input::getMousePos(int width, int height)
{
	static POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(mHwnd, &mousePos);
	return math::vec2(((float)mousePos.x /width - 0.5f)*2.f,
		((float)mousePos.y / height - 0.5f)*2.f);
}

math::vec2 Input::getMouseMovement()
{
	math::vec2 out((float)deltaMouseX, (float)deltaMouseY);
	deltaMouseX = deltaMouseY = 0;
	return out;
}