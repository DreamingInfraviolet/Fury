#pragma once
#include "renderwindow.h"
#include "eventmaster.h"
#include "immediateio.h"
#include "renderstate.h"
#include "scene.h"
#include "mesh.h"
#include "input.h"
#include "texture.h"
#include "shader.h"
#include "timer.h"
#include "game.h"

#include<list>

class System
{
private:
	System()
	{
		mRunning = true;
	}

	RenderWindow mWindow;
	EventMaster mEventMaster;
	bool mRunning;

	friend class DirectX;
public:


	static System* createSystem()
	{
		static bool created = false;
		if (created)
			return NULL;
		created=true;
		return new System();
	}

	bool initialise(HINSTANCE hInstance, LPSTR, int)
	{
		if (
			!mWindow.initialise(hInstance) ||
			!mEventMaster.initialise()
			)
			return false;
		return true;
	}

	void handleMessages()
	{
		Event e;
		while (mWindow.pollEvent(&e))
		{
			switch (e.type)
			{
			case Event::Close:
				if (mWindow.isOpen())
					mWindow.close();
			}
		}
	}

	bool run()
	{
		Timer timer;
		Game game(&mWindow);

		if (!game.initialise())
			return false;

		while (mWindow.isOpen())
		{
			timer.startFrame();
			handleMessages();
			
			if (!game.processInput(timer.deltaTime()))
				break;
			game.frame(timer.deltaTime());
			game.draw();

			mWindow.present();
			timer.endFrame();
		}
		return true;
	}

	void shutdown()
	{
		mWindow.shutdown();
	}

	void stop()
	{
		mWindow.close();
	}
};