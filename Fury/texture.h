#pragma once
#include "rendertarget.h"

//Always 4 components
class Texture
{
protected:
unsigned char* mData;
unsigned mDataSize;
int mHeight;
int mWidth;
bool mInCPU;

public:

enum class TextureType { DIFFUSE, NORMAL, SPECULAR, DEPTH };

Texture();
Texture(const char* file);
Texture(int width, int height, unsigned char * data);
~Texture();

bool inCPU();

bool loadFromFile(const char* file);
bool saveToFile(const char* file);

unsigned char* getData();
int getDataLength();

virtual void clear();
};


class TextureGPU : public Texture
{
protected:
bool mInGPU;
ID3D11Texture2D* mTexture;
ID3D11ShaderResourceView* mView;
ID3D11SamplerState* mSamplerState;

public:
TextureGPU();
TextureGPU(const char* file);
TextureGPU(int width, int height, int components, char* data);

bool upload(ID3D11Device* device, TextureType type);
bool deupload();

ID3D11ShaderResourceView* getTextureView();
bool inGPU();
virtual void clear();

void setForRendering(ID3D11DeviceContext* device);
};


class TextureRenderTarget : public TextureGPU, public RenderTarget
{
private:
bool mIsRenderTarget;

public:
TextureRenderTarget();
TextureRenderTarget(const char* file);
TextureRenderTarget(int width, int height, int components, char* data);

bool isRenderTarget();
bool setAsRenderTarget();
bool unsetAsRenderTarget();
};