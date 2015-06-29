#include "mesh.h"
#include "vmfheader.h"
#include "immediateio.h"
#include <fstream>
#include "directx.h"
#include "renderwindow.h"
#include "camera.h"
#include "shader.h"
#include "shaderbuffertypes.h"

Mesh::Mesh() : m_inCPU(0),  m_vertexCount(0), m_indexCount(0){}

Mesh::Mesh(const char *path) : m_inCPU(0),
							   m_vertexCount(0), m_indexCount(0)
{
	load(path);
}

Mesh::~Mesh()
{
	clear();
}

bool Mesh::load(const char *path)
{
	std::string fileLocation = path;

	Inform(std::string("Loading object: \"") + fileLocation + "\"");

	std::ifstream modelFile(fileLocation, std::ios::binary);
	modelFile.seekg(0);

	if (modelFile.fail())
	{
		Warning(std::string("Could not open \"") + fileLocation + "\"!\n" + strerror(errno));
		return false;
	}

	m_location = path;

	//Read in header
	VMFHeader header;
	modelFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	m_vertexCount	= header.vertCount;
	m_indexCount	= header.indexCount;

	m_vertices.clear();
	m_indices.clear();
	m_inCPU = false;

	//Fill vectors from file:
	if (header.vertCount > 0)
	{
		m_vertices.resize(header.vertCount);
		modelFile.read(reinterpret_cast<char*>(&m_vertices[0]), header.vertCount*sizeof(m_vertices[0]));
	}
	if (header.indexCount > 0)
	{
		m_indices.resize(header.indexCount);
		modelFile.read(reinterpret_cast<char*>(&m_indices[0]), header.indexCount*sizeof(ElementIndexType));
	}
	//

	m_inCPU = true;
	return true;
}

void Mesh::clearCPU()
{
	m_vertices.clear();
	m_indices.clear();
	m_inCPU = false;
}

bool RenderMesh::upload(RenderWindow* window, Enum::Usage vertexUsage)
{
	if (!window)
		return false;

	if (m_inCPU)
	{
		if (m_inGPU)
			return true;

		m_inGPU = true;

		DirectX& dx = window->getDirectX();

		D3D11_BUFFER_DESC vertexBufferDesc = { 0 }, indexBufferDesc = { 0 };
		D3D11_SUBRESOURCE_DATA vertexData, indexData;

		//Set up the vertex buffer
		vertexBufferDesc.Usage = (D3D11_USAGE)vertexUsage;
		vertexBufferDesc.ByteWidth = sizeof(Vertex3D)*m_vertices.size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		vertexData.pSysMem = &m_vertices[0];
		vertexData.SysMemPitch = sizeof(Vertex3D)*m_vertices.size();
		
		if (FAILED(dx.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &gpuInfo.vertexBuffer)))
			return false;

		//Set up the index buffer
		indexBufferDesc.Usage = (D3D11_USAGE)vertexUsage;
		indexBufferDesc.ByteWidth = sizeof(ElementIndexType)*m_indices.size();
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		indexData.pSysMem = &m_indices[0];

		if (FAILED(dx.getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &gpuInfo.indexBuffer)))
			return false;


		return true;
	}
	return false;
}

void RenderMesh::deupload()
{
	if (m_inGPU)
	{
		m_inGPU = false;

		releaseCom(gpuInfo.indexBuffer);
		releaseCom(gpuInfo.vertexBuffer);
	}
}

void RenderMesh::clear()
{
	deupload();
	clearCPU();
}

void Mesh::clear()
{
	clearCPU();
}

void StaticMeshInstance::draw(RenderWindow* renderWindow, const math::mat4& initialTransformMatrix, const Camera& camera)
{
	assert(mParent->inGPU());

	auto context = renderWindow->getDirectX().getContext();
	const unsigned stride = sizeof(Vertex3D), start = 0;

	context->IASetVertexBuffers(0, 1, &mParent->gpuInfo.vertexBuffer, &stride, &start);
	context->IASetIndexBuffer(mParent->gpuInfo.indexBuffer, ELEMENT_INDEX_TYPE_ENUM, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	VertexBufferStaticmesh vertexUniforms;
	vertexUniforms.projection = camera.projection();
	vertexUniforms.view = camera.matrix();
	vertexUniforms.world = math::mat4().initScale(0.2f)*initialTransformMatrix;

	PixelBufferStaticmesh pixelUniforms;
	pixelUniforms.colour = mColour;

	renderWindow->getActiveShader()->setVertexData(context, &vertexUniforms, 0);
	renderWindow->getActiveShader()->setPixelData(context, &pixelUniforms, 0);
	renderWindow->getActiveShader()->render(context, mParent->m_indexCount);
}