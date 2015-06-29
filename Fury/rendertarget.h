#pragma once
#include "drawable.h"
#include "directx.h"
#include "renderstate.h"
#include<vector>
class RenderTarget
{
protected:
	RenderTarget(){}
	

	friend class DirectX;
public:
	std::vector<ID3D11RenderTargetView*> mDXRenderTargets;
	std::vector<ID3D11DepthStencilView*> mDXDepthStencils;
	~RenderTarget(){}

};