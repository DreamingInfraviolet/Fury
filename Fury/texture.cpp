#include "texture.h"
#include "resourceloader.h"
#include "windows.h"
#include "util.h"
#include <fstream>
#include <assert.h>
Texture::Texture()
{
	mData = 0;
	clear();
}

Texture::Texture(const char* file)
{
	mData = 0;
	clear();
	loadFromFile(file);
}

Texture::Texture(int width, int height, unsigned char* data)
{
	clear();
	mWidth = width;
	mHeight = height;
	mData = data;
	
}

Texture::~Texture()
{
	clear();
}

bool Texture::inCPU()
{
	return mInCPU;
}

//Ensures 4 components. Bitmaps seem to be stored in BGR!
bool Texture::loadFromFile(const char* path)
{
	clear();

	if (toLowerCase(ResourceLoader::getFileExtension(path)) != "bmp")
		return false;

	std::fstream file(path, std::ios::in | std::ios::beg | std::ios::binary);

	
	unsigned char header[54];
	file.read((char*)&header, 54);

	if (*(short*)&header[0] != 19778)
		return false;

	unsigned dataPos, dataSize;

	dataPos = *(unsigned*)&(header[0x0A]);
	dataSize = *(unsigned*)&(header[0x22]);
	mWidth = *(unsigned*)&(header[0x12]);
	mHeight = *(unsigned*)&(header[0x16]);

	//If malformed, guess:
	if (dataSize == 0)
		dataSize = mWidth * mHeight * 3;
	if (dataPos == 0) dataPos = 54;

	//mDataSize = final size. dataSize = imported image size.

	unsigned noOfComponents = dataSize / (mWidth*mHeight);

	file.seekg(dataPos);
	mDataSize = mWidth*mHeight * 4 * sizeof(unsigned char);


	//If empty
	if (mDataSize == 0)
		return true;

	mData = new unsigned char[mDataSize];

	//If not 4 components, convert!
	if (noOfComponents != 4)
	{
		unsigned char fillInColour = 0;
		unsigned char* tmpData = new unsigned char [dataSize];
		file.read((char*)tmpData, dataSize);

		//Now convert tmpData to mData:
		switch (noOfComponents)
		{
		case 1:
			//Use as greyscale:
			for (unsigned i = 0; i < dataSize; ++i)
			{
				int i4 = i * 4;
				mData[i4] = tmpData[i];
				mData[i4 + 1] = tmpData[i];
				mData[i4 + 2] = tmpData[i];
				mData[i4 + 3] = tmpData[i];
			}
			break;
		case 2:
			//For the rest, fill in blanks with fillInColour:
			for (unsigned i = 0, j = 0; i < dataSize; i += 2, j += 4)
			{
				mData[j] = tmpData[i];
				mData[j + 1] = tmpData[i+1];
				mData[j + 2] = fillInColour;
				mData[j + 3] = fillInColour;
			}
			break;
		case 3:
			for (unsigned i = 0, j=0; i < dataSize; i += 3, j+=4)
			{
				mData[j] = tmpData[i];
				mData[j + 1] = tmpData[i + 1];
				mData[j + 2] = tmpData[i + 2];
				mData[j + 3] = fillInColour;
			}
			break;
		default:
			assert(0);
		}
	}
	else
	{
		file.read((char*)mData, mDataSize);
	}

	file.close();
	mInCPU = true;

	return true;
}

bool Texture::saveToFile(const char* file)
{
	return false;
}

unsigned char* Texture::getData()
{
	return mData;
}

int Texture::getDataLength()
{
	return mDataSize;
}

void Texture::clear()
{
	if (mData)
		delete[] mData;
	mWidth = mHeight = 0;
	mData = NULL;
	mInCPU = false;
}


TextureGPU::TextureGPU() : mView(0), mTexture(0)
{
	
}

TextureGPU::TextureGPU(const char* file) : mView(0), mTexture(0)
{

}

TextureGPU::TextureGPU(int width, int height, int components, char* data) : mView(0), mTexture(0)
{

}

bool TextureGPU::upload(ID3D11Device* device, TextureType type)
{
	if (mView) //update
	{

	}
	else //upload
	{
		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = mWidth;
		textureDesc.Height = mHeight;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.MipLevels = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.ArraySize = 1;


		D3D11_SUBRESOURCE_DATA textureData;
		textureData.pSysMem = mData;
		textureData.SysMemPitch = sizeof(mData[0]) * mWidth * 4; //width bytes
		textureData.SysMemSlicePitch = mDataSize; //Total bytes

		if (FAILED(device->CreateTexture2D(&textureDesc, &textureData, &mTexture)))
			return false;

		if (FAILED(device->CreateShaderResourceView(mTexture, NULL, &mView)))
			return false;

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		if (FAILED(device->CreateSamplerState(&samplerDesc, &mSamplerState)))
			return false;
	}
	return true;
}

void TextureGPU::setForRendering(ID3D11DeviceContext* context)
{
	context->PSSetShaderResources(0, 1, &mView);
	context->PSSetSamplers(0, 1, &mSamplerState);
}

ID3D11ShaderResourceView* TextureGPU::getTextureView()
{
	return mView;
}

bool TextureGPU::deupload()
{
	releaseCom(mView);
	releaseCom(mTexture);
	return true;
}

bool TextureGPU::inGPU()
{
	return false;
}

void TextureGPU::clear()
{
	deupload();
	Texture::clear();
}