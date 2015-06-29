#pragma once
#include "window.h"
#include "graphicsinfo.h"
#include "rendertarget.h"

class Scene;

class RenderWindow : public Window, public RenderTarget
{
private:
	DirectX directx;
	RenderState renderState;
	GraphicsInfo graphicsInfo;
	ShaderProgram* activeShader;

public:
	RenderWindow() : activeShader(NULL) {}

	virtual bool initialise(HINSTANCE hInstance) 
	{
		if(!Window::initialise(hInstance) ||
			!directx.initialise(mWidth, mHeight, mHwnd, &graphicsInfo))
			return false;
		return true;
	}

	virtual void shutdown()
	{
		directx.shutdown();
		Window::shutdown();
	}

	void setActiveShader(ShaderProgram* shader)
	{
		activeShader = shader;
	}

	ShaderProgram* getActiveShader()
	{
		return activeShader;
	}

	void setRenderTarget(RenderTarget* target)
	{
		directx.setRenderTarget(target);
	}

	void present()
	{
		directx.present();
	}

	void draw(Scene* scene);

	void clear(float r=0, float g=0, float b=0)
	{
		directx.clear(r,g,b);
	}

	DirectX& getDirectX()
	{
		return directx;
	}
};