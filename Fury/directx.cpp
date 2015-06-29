#include "renderstate.h"
#include "renderwindow.h"
#include "rendertarget.h"
#include "graphicsinfo.h"
#include <assert.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "D3DCompiler.lib")


std::string blobToString(ID3D10Blob* blob)
{
	if (!blob)
		return "";
	return stringFromStrLen((char*)blob->GetBufferPointer(), blob->GetBufferSize());
}



bool DirectX::initialise(int width, int height, HWND hwnd, GraphicsInfo* gInfo)
{
	if (
		!(
		updateGraphicsInfo(gInfo) &&
		createDeviceAndSwapChain(width, height, hwnd) &&
		createRenderTargetView() &&
		createDepthBuffer(width, height) &&
		createDepthStencilState() &&
		createDepthStencilView() &&
		setRenderTarget(NULL) &&
		createRasterState() &&
		createViewport(width, height)
		)
		)
		return false;


	return true;
}


bool DirectX::createDeviceAndSwapChain(int width, int height, HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };

	swapChainDesc.BufferCount = 1; //Number of back buffers!
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

#ifdef _DEBUG
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &mDevice, NULL, &mContext)))
#else
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &mDevice, NULL, &mContext)))
#endif
		return false;

	return true;
}


bool DirectX::createRenderTargetView()
{
	if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mBackBuffer)))
		return false;

	if (FAILED(mDevice->CreateRenderTargetView(mBackBuffer, NULL, &mRenderTargetView)))
		return false;

	return true;
}


bool DirectX::createDepthBuffer(int width, int height)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc = { 0 };
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	if (FAILED(mDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer)))
		return false;

	return true;
}


bool DirectX::createDepthStencilState()
{
	//Stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HRESULT result;
	if (FAILED(result = mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState)))
		return false;
	mContext->OMSetDepthStencilState(mDepthStencilState, 1);
	return true;
}


bool DirectX::createDepthStencilView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(mDevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView)))
		return false;
	return true;
}

bool DirectX::createViewport(int width, int height)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	mContext->RSSetViewports(1, &viewport);
	return true;
}


bool DirectX::createRasterState()
{
	//Set up rasterised state, which allows for greater control over rasterisation.

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.f;

	if (FAILED(mDevice->CreateRasterizerState(&rasterDesc, &mRasterState)))
		return false;

	//mContext->RSSetState(mRasterState);

	return true;
}


bool DirectX::updateGraphicsInfo(GraphicsInfo* gInfo)
{
	if (!gInfo)
		return false;

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;

	if(FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)&factory)))
		return false;

	//Create an adapter for primary gpu interface:
	if (FAILED(factory->EnumAdapters(0,&adapter)))
		return false;

	if(FAILED(adapter->EnumOutputs(0,&adapterOutput)))
		return false;

	//Find number of modes that fit DXGI_FORMAT_R8G8B8A8_UNORM
	UINT numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
		return false;

	//List of all modes:
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];

	UINT numModes2 = numModes;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes2, displayModeList)))
		return false;

	gInfo->graphicsModes.resize(numModes);

	for (UINT i = 0; i < numModes; ++i)
	{
		gInfo->graphicsModes[i].width = (int)displayModeList[i].Width;
		gInfo->graphicsModes[i].height = (int)displayModeList[i].Height;
		gInfo->graphicsModes[i].refreshRate = (float)displayModeList[i].RefreshRate.Numerator/displayModeList[i].RefreshRate.Denominator;
		gInfo->graphicsModes[i].refreshRateNumerator = displayModeList[i].RefreshRate.Numerator;
		gInfo->graphicsModes[i].refreshRateDenominator = displayModeList[i].RefreshRate.Denominator;
	}

	//Get GPU info :D
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc)))
		return false;

	gInfo->card.description = adapterDesc.Description;
	gInfo->card.vendorID = adapterDesc.VendorId;
	gInfo->card.deviceID = adapterDesc.DeviceId;
	gInfo->card.revision = adapterDesc.Revision;
	gInfo->card.videoMemory = adapterDesc.DedicatedVideoMemory;
	gInfo->card.systemMemory = adapterDesc.DedicatedSystemMemory;
	gInfo->card.sharedSystemMemory = adapterDesc.SharedSystemMemory;

	delete[] displayModeList;
	adapterOutput->Release();
	adapter->Release();
	factory->Release();

	return true;
}

void DirectX::shutdown()
{

	releaseCom(mRasterState);
	releaseCom(mDepthStencilView);
	releaseCom(mDepthStencilState);
	releaseCom(mDepthStencilBuffer);
	releaseCom(mBackBuffer);
	releaseCom(mRenderTargetView);
	releaseCom(mContext);
	releaseCom(mDevice);
	if (mSwapChain)
	{
		mSwapChain->SetFullscreenState(false, NULL);
		mSwapChain->Release();
	}
}

void DirectX::resize(){}

void DirectX::clear(float r, float g, float b, bool clearDepth, bool clearStencil)
{
	float colour[] = {r,g,b,0};
	mContext->ClearRenderTargetView(mRenderTargetView,colour);
	mContext->ClearDepthStencilView(mDepthStencilView, (clearDepth ? D3D11_CLEAR_DEPTH:0) | (clearStencil ? D3D11_CLEAR_STENCIL:0), 1.f, 0);
}

void DirectX::present()
{
	mSwapChain->Present(0,0);
}

bool DirectX::setRenderTarget(RenderTarget* target)
{
	if(!target)
		mContext->OMSetRenderTargets(1,&mRenderTargetView,mDepthStencilView);
	else
	{
		assert(target->mDXDepthStencils.size()==target->mDXRenderTargets.size());
		mContext->OMSetRenderTargets(target->mDXDepthStencils.size(),&target->mDXRenderTargets[0], target->mDXDepthStencils[0]);
	}
	return true;
}

ID3D11Device* DirectX::getDevice()
{
	return mDevice;
}

ID3D11DeviceContext* DirectX::getContext()
{
	return mContext;
}