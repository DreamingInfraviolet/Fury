#pragma once
#include "directx.h"
#include "immediateio.h"
#include "renderwindow.h"
#include "util.h"
#include <d3dcompiler.h>
#include <d3d11.h>
#include <assert.h>
#include "resourceloader.h"

#include "enum.h"
#include "util.h"
#include "directx.h"
#include "immediateio.h"
#include "shaderbuffertypes.h"

namespace ShaderOP
{

	/**
	* The various shader types that are possible.
	*/
	enum ShaderType { TYPE_DOMAIN, TYPE_GEOMETRY, TYPE_HULL, TYPE_PIXEL, TYPE_VERTEX, TYPE_COMPUTE };


	/**
	* Prints a shader error using the "Error()" function.
	@param blob The DirectX blob containing the error message.
	*/
	void printShaderError(ID3D10Blob* blob);


	/**
	* Compiles shader source to bytecode.
	* @param file The path of the shader file to be compiled.
	* @param shaderBlob A reference to the blob that is to hold the compiled bytecode.
	* @param type The type of the shader that is to be compiled. e.g., TYPE_VERTEX.
	* @return result Returns true if the complation was successful.
	*/
	bool compileToBytecode(const std::string& file, ID3D10Blob*& shaderBlob, ShaderType type);
}


/**
* This class helps to manage constant buffers, allowing data to be sent to the appropriate GPU constant buffer.
*/
class ConstantBuffer
{
private:
	size_t mSize;

public:
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ID3D11Buffer* mBuffer;
	
	/**
	* The default constructor creates the constant buffer in video memory.
	* @todo Perhaps move this functionality to an initialisation method.
	* @param usage The intended usage patterns. e.g., USAGE_DYNAMIC.
	* @param The DirectX device pointer.
	* @param dataSize The size of the buffer structure.
	* @todo Determine what the id does....
	*/
	ConstantBuffer();

	bool initialise(Enum::Usage usage, ID3D11Device* device, size_t dataSize);

	/**
	* Frees the constant buffer from video memory.
	*/
	~ConstantBuffer();


	/**
	* Returns the size of the buffer, as set during creation.
	*/
	size_t size() { return mSize; }


	/**
	* Returns the internal ID3D11 buffer representation.
	*/
	ID3D11Buffer* getBuffer() { return mBuffer; }


	/**
	* Returns true if the buffer is inside the GPU.
	*/
	bool isLoaded() { return mBuffer != NULL; }


	/**
	* Locks the buffer so that it can be written to (write-only).
	* @param context The current DirectX context.
	*/
	bool lock(ID3D11DeviceContext* context);


	/**
	* Unlocks the buffer, prohibiting user write and allowing GPU use.
	* @param context The current DirectX context.
	*/
	bool unlock(ID3D11DeviceContext* context);

	bool setdata(const void * data);


	/**
	* Place data into the buffer's GPU memory. sizeof(*data) must be equal to the size specified upon creation.
	* @param context The current DirectX context.
	*/
	bool setdata(ID3D11DeviceContext* context, const void* data);

};



/**
* This class is responsible for specifying the input layout of the shader.
*/
class InputLayout
{
private:
	ID3D11InputLayout* mLayout;
public:
	enum Type { POSITION, NORMAL, TEXCOORD, TYPE_COUNT };

	/**
	* A constructor initialising everything to its default value.
	*/
	InputLayout() : mLayout(NULL) {}


	/**
	* A destructor that frees the layout from video memory.
	*/
	~InputLayout(){ clear(); }


	/**
	* Creates the input layout, based on a vector that describes it in detail.
	* @param layout The layout description.
	* @param device The ID3D11 device.
	* @param shaderBlob A pointer to the compiled shader bytecode.
	* @return Returns true if the operation was successful.
	*/
	bool create(const std::vector<Type> layout, ID3D11Device* device, ID3D10Blob*& shaderBlob);


	/**
	* Returns the internal layout representation.
	*/
	ID3D11InputLayout* getLayout()
	{
		return mLayout;
	}

	/**
	* Clears the internal layout representation.
	*/
	void clear()
	{
		releaseCom(mLayout);
	}
};



/**
* This class provides essential shader functionality. All shaders are derived from it.
*/
class ShaderBase
{
protected:
	ID3D10Blob* mShaderBlob;
	std::vector<ConstantBuffer*> mConstantBuffers;

	/**
	* Compiles the bytecode of the shader, preparing it for use.
	* @param shaderBlob A reference to the blob that is to hold the compiled bytecode.
	* @param type The type of the shader that is to be compiled. e.g., TYPE_VERTEX.
	* @return result Returns true if the complation was successful.
	*/
	bool loadGeneric(const std::string& path, ShaderOP::ShaderType type) { return ShaderOP::compileToBytecode(path, mShaderBlob, type); }


public:
	/**
	* The constructor only does basic default value initialisation.
	*/
	ShaderBase() : mShaderBlob(0) {}


	/**
	* Deletes the internal constant buffers.
	*/
	~ShaderBase();


	/**
	* Attaches a constant buffer to the shader. The buffer will be deleted upon destruction.
	* The buffer is added linearly in an array.
	* @return The id of the buffer, in order to refer to it later within the shader.
	*/
	size_t addConstantBuffer(ConstantBuffer* buffer);

	
	/**
	* Returns the number of constant buffers attached to the shader.
	*/
	size_t getBufferCount() { return mConstantBuffers.size(); }


	/**
	* Sets the data for a particular constant buffer.
	* @param context A pointer to the ID3D11 Device context.
	* @param data The single data structure to be sent.
	* @param bufferNo The id of the buffer, as returned by addConstantBuffer(...).
	* @return Returns 0 upon success. 1 if the buffer data was not set. 2 if the CBuffer index is invalid. 3 if there was a fatal error.
	*/
	template<class DataType>
	int setConstantBuffersData(ID3D11DeviceContext* context, DataType* data, size_t bufferNo)
	{
		//If there is no data in the pointer, or the data struct is empty.
		if (sizeof(DataType) < 2 || !data) 
			return 1;

		if (bufferNo >= mConstantBuffers.size())
			return 2;

		assert(mConstantBuffers[bufferNo]->size() == sizeof(DataType)); //Can't just change data type ^^

		if (
			!mConstantBuffers[bufferNo]->lock(context) ||
			!mConstantBuffers[bufferNo]->setdata(data) ||
			!mConstantBuffers[bufferNo]->unlock(context))
			return 3;
		
		return 0;
	}


	/**
	* Clears the shader, reverting it to its starting state.
	*/
	void clear();


	/**
	* A loading method that performs shader type-specific preparation. Use this when loading a shader.
	* @param path The path of the shader file.
	* @param device A pointer to the ID3D11 Device.
	*/
	virtual bool load(const std::string& path, ID3D11Device* device) = 0;


	/**
	* Sets it as the current pixel shader for rendering.
	*/
	virtual bool setForRendering(ID3D11DeviceContext* context) = 0;


	/**
	* Returns true if the shader is ready to be used.
	*/
	virtual bool isLoaded() = 0;
};



/**
* This class overrides the ShaderBase to implement pixel shader specific functionality.
*/
class PixelShader : public ShaderBase
{
private:
	ID3D11PixelShader* mShader;

public:

	/**
	* Initialises internal values to zero.
	*/
	PixelShader() : ShaderBase(), mShader(0) { mShader = NULL; }

	
	/**
	* Releases the internal shader representation.
	*/
	~PixelShader() { releaseCom(mShader); }

	
	/**
	* Returns the internal shader representation.
	*/
	ID3D11PixelShader* getShader() { return mShader; }


	/**
	* Loads the shader from a file.
	* @param path The path of the shader file.
	* @param device The DirectX device.
	*/
	virtual bool load(const std::string& path, ID3D11Device*  device);


	/**
	* Sets it as the current pixel shader for rendering.
	*/
	virtual bool setForRendering(ID3D11DeviceContext* context);


	/**
	* Returns true if the shader is ready to be used.
	*/
	virtual bool isLoaded()
	{
		return mShader != NULL;
	}
};


/**
* This class overrides the ShaderBase to implement vertex shader specific functionality.
*/
class VertexShader : public ShaderBase
{
private:
	ID3D11VertexShader* mShader;
	InputLayout mLayout;
public:

	/**
	* Initialises internal values to zero.
	*/
	VertexShader() : ShaderBase(), mShader(0) {}


	/**
	* Releases the internal shader representation.
	*/
	~VertexShader() { releaseCom(mShader); }


	/**
	* Returns the currently set vertex shader layout.
	*/
	ID3D11InputLayout* getLayout()
	{
		return mLayout.getLayout();
	}


	/**
	* Returns the internal shader represenation.
	*/
	ID3D11VertexShader* getShader()
	{
		return mShader;
	}


	/**
	* A loading method that performs shader type-specific preparation. Use this when loading a shader.
	* @param path The path of the shader file.
	* @param device The DirectX device.
	*/
	virtual bool load(const std::string& path, ID3D11Device* device);


	/**
	* Sets it as the current pixel shader for rendering.
	*/
	virtual bool setForRendering(ID3D11DeviceContext* context);


	/**
	* Returns true if the shader is ready to be used.
	*/
	virtual bool isLoaded()
	{
		return mShader != NULL;
	}
};



/**
* A class that is used when constructing an entire shader program object to save time.
*/
struct ShaderProgramDescriptor
{
	std::string vertexPath, pixelPath;

	enum Token { T_START_CBUFFER, T_END_CBUFFER, T_FLOAT, T_FLOAT2, T_FLOAT4, T_MATRIX_4 };

	std::vector<Token> mPixelDesc, mVertexDesc;

	/**
	* A default constructor that sets the internal values to the parameters.
	*/
	ShaderProgramDescriptor(const std::string& vertexPath_, const std::string& pixelPath_,
		const std::initializer_list<Token>& vertexDesc, const std::initializer_list<Token>& pixelDesc);


	/**
	* Uses its internal state to construct GPU constant buffers for the vs and ps shaders.
	* @param vs A pointer to the vertex shader object.
	* @param fs A pointer to the fragment shader object.
	* @param device A pointer to the DirectX Device.
	*/
	bool constructCbuffersForShaders(VertexShader* vs, PixelShader* ps, ID3D11Device* device);

private:

	/**
	* An internally used object to represent a single constant buffer.
	*/
	struct CBufferDesc
	{
		std::vector<Token> tokens;
		int expectedSize;

		CBufferDesc() { reset(); }

		void reset()
		{
			tokens.clear();
			expectedSize = 0;
		}
	};


	/**
	* An internal method that fills a CBufferDesc object from the token stream being currently read. Automatically advances index.
	* The index is now beyond T_END_CBUFFER if it succeeded.
	* @param cb A pointer to the constant buffer to be filled.
	* @param desc The stream of tokens being read.
	* @param index The index marking the start of the buffer in the stream of tokens.
	* @return status Returns true if succeeded.
	*/
	bool fillCbufferDesc(CBufferDesc* cb, std::vector<Token>& desc, size_t& index);


	/**
	* Constructs the constant buffers for a generic shader.
	* @param s The shader to which the constant buffers are to be added.
	* @param desc The stream of tokens describing the buffers.
	* @param device A pointer to the ID3D11 Device.
	* @param status Returns true if successful.
	*/
	bool constructCbuffersForShader(ShaderBase* s, std::vector<Token>& desc, ID3D11Device* device);
};


/**
* A class that greatly simplifies the creation of shader programs.
*/
class ShaderProgram
{
protected:
	
	VertexShader* mVertexShader;
	PixelShader* mPixelShader;

public:
	/**
	* Loads and creates a shader program based on the input descriptor.
	* @param desc The object describing the shader program.
	* @param device A pointer to the ID3D11 Device.
	* @return status Returns true if successful.
	*/
	bool load(ShaderProgramDescriptor* desc, ID3D11Device* device);


	/**
	* Destroys the two internal shader objects.
	*/
	~ShaderProgram();


	/**
	* Sets a cbuffer for the vertex shader.
	*/
	template<class VertexDataType >
	void setVertexData(ID3D11DeviceContext* context, const VertexDataType* vertexdata, size_t index)
	{
		mVertexShader->setConstantBuffersData(context, vertexdata, index);
	}

	/**
	* Sets a cbuffer for the pixel shader.
	*/
	template<class PixelDataType >
	void setPixelData(ID3D11DeviceContext* context, const PixelDataType* pixeldata, size_t index)
	{
		mPixelShader->setConstantBuffersData(context, pixeldata, index);
	}


	/**
	* Sets the program as active and draws the currently set objects.
	* @param context The DirectX Context.
	* @param indexCount The number of indices to draw.
	*/
	void render(ID3D11DeviceContext* context, int indexCount);
};