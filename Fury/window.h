#define WIN32_LEAN_AND_MEAN
#include "immediateio.h"
#include <windows.h>
#include "event.h"
#include "input.h"

class Window
{
protected:

	friend class EventMaster;
	Input mInput;
public:

	Window();

	virtual bool initialise(HINSTANCE hInstance);

	virtual void shutdown();

	bool isOpen();

	void close();

	bool pollEvent(Event* e);

	void show();
	void hide();
	void resize();

	int getHeight(){ return mHeight; }
	int getWidth() { return mWidth; }

	HWND getHwnd();

	Input& getInput();

protected:

	

	HWND mHwnd;
	int mWidth,mHeight;
	bool mIsOpen;

	static Event mTmpEvent;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

extern Window gWindow;