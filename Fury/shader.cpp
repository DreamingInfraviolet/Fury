#include "shader.h"

namespace ShaderOP
{
	void printShaderError(ID3D10Blob* blob)
	{
		char* compileErrors = (char*)(blob->GetBufferPointer());
		unsigned long bufferSize = blob->GetBufferSize();
		std::string text = stringFromStrLen(compileErrors, bufferSize);
		Error(text);
	}


	bool compileToBytecode(const std::string& file, ID3D10Blob*& shaderBlob, ShaderType type)
	{
		std::string target;

		switch (type)
		{
		case TYPE_COMPUTE:
			target = "cs";
			break;
		case TYPE_DOMAIN:
			target = "ds";
			break;
		case TYPE_GEOMETRY:
			target = "gs";
			break;
		case TYPE_HULL:
			target = "hs";
			break;
		case TYPE_PIXEL:
			target = "ps";
			break;
		case TYPE_VERTEX:
			target = "vs";
			break;
		}

		target += "_5_0";

		
		HRESULT result = 0;
		ID3D10Blob* errorBlob = NULL;

		//Other interesting flags1: D3DCOMPILE_OPTIMIZATION_LEVEL3

		if (FAILED(result = D3DCompileFromFile(stringToWstring(file).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target.c_str(), D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS, 0, &shaderBlob, &errorBlob)))
		{
#pragma warning(suppress: 6102)
			if (errorBlob)
			{
				printShaderError(errorBlob);
				errorBlob->Release();
			}
			else
			{
				Error(std::string("Error creating shader:\nCould not open \"") + file + "\"");
			}
			return false;
		}

		return true;
	}
}


ConstantBuffer::ConstantBuffer() {}


bool ConstantBuffer::initialise(Enum::Usage usage, ID3D11Device* device, size_t dataSize)
{
	mSize = dataSize;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = (D3D11_USAGE)usage;
	bufferDesc.ByteWidth = dataSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&bufferDesc, NULL, &mBuffer)))
	{
		Error(std::string("Could not create constant buffer."));
		mBuffer = NULL;
		mSize = 0;
		return false;
	}

	return true;
}


ConstantBuffer::~ConstantBuffer()
{
	releaseCom(mBuffer);
}


bool ConstantBuffer::lock(ID3D11DeviceContext* context)
{
	if (FAILED(context->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		return false;
	return true;
}


bool ConstantBuffer::unlock(ID3D11DeviceContext* context)
{
	context->Unmap(mBuffer, 0);
	return true;
}


bool ConstantBuffer::setdata(const void* data)
{
	memcpy(mappedSubresource.pData, data, mSize);
	return true;
}


ShaderBase::~ShaderBase()
{
	clear();
}

void ShaderBase::clear()
{
	releaseCom(mShaderBlob);
	for (auto it = mConstantBuffers.begin(); it != mConstantBuffers.end(); ++it)
		delete (*it);
}

size_t ShaderBase::addConstantBuffer(ConstantBuffer* buffer)
{
	assert(buffer);
	mConstantBuffers.push_back(buffer);
	return mConstantBuffers.size() - 1;
}


bool PixelShader::load(const std::string& path, ID3D11Device*  device)
{
	if (!loadGeneric(path, ShaderOP::TYPE_PIXEL))
		return false;

	HRESULT error;
	if (FAILED(error = device->CreatePixelShader(
		mShaderBlob->GetBufferPointer(), mShaderBlob->GetBufferSize(), NULL, &mShader)))
	{
		Error(std::string("Could not create pixel shader: Error code ") + toString(error, Enum::Hex));
		return false;
	}

	return true;
}

bool PixelShader::setForRendering(ID3D11DeviceContext* context)
{
	for (size_t i = 0; i < mConstantBuffers.size(); ++i)
	{
		ID3D11Buffer* buffer = mConstantBuffers[i]->getBuffer();
		context->PSSetConstantBuffers(i, 1, &buffer);
	}

	context->PSSetShader(mShader, NULL, 0);

	return true;
}

bool InputLayout::create(const std::vector<Type> layout, ID3D11Device* device, ID3D10Blob*& shaderBlob)
{
	D3D11_INPUT_ELEMENT_DESC* layoutDesc = new D3D11_INPUT_ELEMENT_DESC[layout.size()];

	int semanticIndex[TYPE_COUNT];
	for (size_t i = 0; i < TYPE_COUNT; ++i)
		semanticIndex[i] = 0;

	for (size_t i = 0; i < layout.size(); ++i)
	{
#pragma warning(suppress: 6386)
		layoutDesc[i] = { 0 };
		layoutDesc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layoutDesc[i].SemanticIndex = semanticIndex[layout[i]]++;
		
		if(i!=0)
			layoutDesc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		switch (layout[i])
		{
		case POSITION:
			layoutDesc[i].SemanticName = "POSITION";
			layoutDesc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case NORMAL:
			layoutDesc[i].SemanticName = "NORMAL";
			layoutDesc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case TEXCOORD:
			layoutDesc[i].SemanticName = "TEXCOORD";
			layoutDesc[i].Format = DXGI_FORMAT_R32G32_FLOAT;
			break;
		default:
			assert(0);
		}
	}

	HRESULT error;
	if (FAILED(error = device->CreateInputLayout(
		layoutDesc, layout.size(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &mLayout)))
	{
		delete[] layoutDesc;
		Error(std::string("Could not create input layout: Error code ") + toString(error, Enum::Hex));
		return false;
	}

	delete[] layoutDesc;

	return true;
}

bool VertexShader::load(const std::string& path, ID3D11Device*  device)
{
	if (!loadGeneric(path, ShaderOP::TYPE_VERTEX))
		return false;

	HRESULT error;
	if (FAILED(error = device->CreateVertexShader(
		mShaderBlob->GetBufferPointer(), mShaderBlob->GetBufferSize(), NULL, &mShader)))
	{
		Error(std::string("Could not create vertex shader: Error code ") + toString(error, Enum::Hex));
		return false;
	}

	//MUST BE IN ORDER >.< :
	mLayout.create({ InputLayout::POSITION, InputLayout::NORMAL, InputLayout::TEXCOORD}, device, mShaderBlob);
	return true;
}

bool VertexShader::setForRendering(ID3D11DeviceContext* context)
{
	for (size_t i = 0; i < mConstantBuffers.size(); ++i)
	{
		auto buffer = mConstantBuffers[i]->getBuffer();
		context->VSSetConstantBuffers(i, 1, &buffer);
	}
	
	context->IASetInputLayout(mLayout.getLayout());
	context->VSSetShader(mShader, NULL, 0);

	return true;
}

ShaderProgramDescriptor::ShaderProgramDescriptor(const std::string& vertexPath_, const std::string& pixelPath_,
	const std::initializer_list<Token>& vertexDesc, const std::initializer_list<Token>& pixelDesc)
	: vertexPath(vertexPath_), pixelPath(pixelPath_), mVertexDesc(vertexDesc), mPixelDesc(pixelDesc) {}

bool ShaderProgramDescriptor::constructCbuffersForShaders(VertexShader* vs, PixelShader* ps, ID3D11Device* device)
{
	if (!constructCbuffersForShader(vs, mVertexDesc, device) || !(constructCbuffersForShader(ps, mPixelDesc, device)))
		return false;
	else
		return true;
}

bool ShaderProgramDescriptor::fillCbufferDesc(CBufferDesc* cb, std::vector<Token>& desc, size_t& index)
{
	assert(desc[index] == T_START_CBUFFER);
	int& size = cb->expectedSize;

	while (++index != desc.size())
	{
		switch (desc[index])
		{
		case T_START_CBUFFER:
			return false;
			break;
		case T_END_CBUFFER:
			++index;
			return true;
		case T_FLOAT:
			cb->tokens.push_back(T_FLOAT);
			size += sizeof(float);
			break;
		case T_FLOAT2:
			cb->tokens.push_back(T_FLOAT2);
			size += sizeof(float) * 2;
			break;
		case T_FLOAT4:
			cb->tokens.push_back(T_FLOAT4);
			size += sizeof(float) * 4;
			break;
		case T_MATRIX_4:
			cb->tokens.push_back(T_MATRIX_4);
			size += sizeof(math::mat4);
			break;
		default:
			assert(0);
		}
	}

	//If still here, no cbuffer closing token...
	return false;
}

bool ShaderProgramDescriptor::constructCbuffersForShader(ShaderBase* s, std::vector<Token>& desc, ID3D11Device* device)
{
	CBufferDesc cbd;

	for (size_t iToken = 0; iToken < desc.size(); ++iToken)
	{
		if (desc[iToken] == T_START_CBUFFER)
		{
		
			cbd.reset();
			if (!fillCbufferDesc(&cbd, desc, iToken))
				return false;
			else
			{
				ConstantBuffer* buffer = new ConstantBuffer();

				if (!buffer || !buffer->initialise(Enum::USAGE_DYNAMIC, device, cbd.expectedSize))
					return false;

				s->addConstantBuffer(buffer);

				//Note how only the expected size of the CBufferDesc is used. This is fine, but could be used.
			}
		}
	}
	return true;
}

ShaderProgram::~ShaderProgram()
{
	deletePtr(mVertexShader);
	deletePtr(mPixelShader);
}

void ShaderProgram::render(ID3D11DeviceContext* context, int indexCount)
{
	mVertexShader->setForRendering(context);
	mPixelShader->setForRendering(context);

	context->DrawIndexed(indexCount, 0, 0);
}

bool ShaderProgram::load(ShaderProgramDescriptor* desc, ID3D11Device* device)
{
	mVertexShader = new VertexShader();
	mPixelShader = new PixelShader();

	if (!mVertexShader->load(desc->vertexPath, device) ||
		!mPixelShader->load(desc->pixelPath, device))
		return false;

	//Parse descriptor layout:
	desc->constructCbuffersForShaders(mVertexShader, mPixelShader, device);

	return true;
}