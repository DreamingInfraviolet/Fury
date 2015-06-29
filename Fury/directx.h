#pragma once

#include "windows.h"
#include <d3d11.h>
#include <string>
#include "util.h"

namespace Enum
{
	enum Usage { USAGE_DYNAMIC = D3D11_USAGE_DYNAMIC, USAGE_DEFAULT = D3D11_USAGE_DEFAULT, USAGE_STAGING = D3D11_USAGE_STAGING, USAGE_IMMUTABLE = D3D11_USAGE_IMMUTABLE };
}

template<class T>
void releaseCom(T& ptr)
{
	if (ptr)
	{
		ptr->Release();
		ptr = NULL;
	}
}

std::string blobToString(ID3D10Blob* blob);


class RenderState;
class RenderWindow;
class RenderTarget;
class ShaderProgram;
class GraphicsInfo;

class DirectX
{
private:
	HWND mHwnd = 0;
	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mContext = nullptr;
	ID3D11RenderTargetView* mRenderTargetView = nullptr;
	ID3D11DepthStencilView* mDepthStencilView = nullptr;
	ID3D11DepthStencilState* mDepthStencilState = nullptr;
	ID3D11Texture2D* mBackBuffer = nullptr;
	ID3D11Texture2D* mDepthStencilBuffer = nullptr;
	ID3D11RasterizerState* mRasterState = nullptr;


	bool createRenderTargetView();
	bool createDepthStencilView();
	bool createViewport(int width, int height);
	bool createDepthStencilState();
	bool createRasterState();
	bool createDepthBuffer(int width, int height);
	bool createDeviceAndSwapChain(int width, int height, HWND hwnd);

	//I really don't want to copy this by accident...
	DirectX(const DirectX&){}

public:

	DirectX() {}
	bool initialise(int width, int height, HWND hwnd, GraphicsInfo* gInfo);

	void shutdown();

	bool updateGraphicsInfo(GraphicsInfo* gInfo);

	void resize();


	//Sets a render target. If input is NULL, resets default render target.
	bool setRenderTarget(RenderTarget*);

	void setShader(ShaderProgram* shader);

	void clear(float r, float g, float b, bool clearDepth = true, bool clearStencil = true);

	void present();

	ID3D11Device* getDevice();

	ID3D11DeviceContext* getContext();

};