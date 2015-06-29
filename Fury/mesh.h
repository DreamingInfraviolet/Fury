#pragma once
#include <vector>
#include <string>
#include "matrixd.h"
#include "texture.h"
#include "vertex.h"
#include "transformable.h"
#include "light.h"
#include "raycasting.h"

/**A virtual base class designed to handle basic mesh management, such as loading, uploading to the GPU, etc. Not placable.*/
class Mesh
{
protected:
	std::vector < Vertex3D > m_vertices;
	std::vector < ElementIndexType > m_indices;
	std::string m_location;
	unsigned m_vertexCount, m_indexCount;
	bool m_inCPU;

public:
	/**Initialises internal variables to zero.*/
	Mesh();

	/**
	Loads an object from the corresponding vmf file during construction.
	@param path The path of the original file (.obj, etc.)
	@param loadLocation A combination of Enum::GPU and Enum::CPU to determine where the model data will be stored.
	*/
	Mesh(const char *path);

	virtual ~Mesh();

	/**
	Loads a model from a file into the CPU. Shouldn't call directly.
	@param path The path of the original file (.obj, etc.)
	*/
	bool load(const char *path);

	std::string location()
	{
		return m_location;
	}

	virtual void clear();

	void clearCPU();
};


class RenderMesh : public Mesh
{
protected:
	bool m_inGPU;

	struct
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
	} gpuInfo;

public:
	RenderMesh() : Mesh(), m_inGPU(0) {}
	RenderMesh(const char *path) : Mesh(path), m_inGPU(0) {}

	bool upload(RenderWindow* window, Enum::Usage vertexUsage = Enum::USAGE_DEFAULT);
	void deupload();

	virtual void clear();
	bool inGPU()
	{
		return m_inGPU;
	}
};




class StaticMesh;

class StaticMeshInstance : public Drawable
{
protected:
	friend class StaticMesh;

	TextureGPU mTexture;
	StaticMesh* mParent;
	math::vec4 mColour;

	StaticMeshInstance(StaticMesh* parent) : Drawable(Drawable::TYPE_STATICMESH)
	{
		mParent = parent;
		mColour = math::vec4(1, 1, 1, 1);
	}

public:

	virtual void draw(RenderWindow* renderWindow, const math::mat4& initialTransformMatrix, const Camera& camera);

	//void setAffectingLight(Light* light)
	//{

	//}

	void setColour(const math::vec4& c)
	{
		mColour = c;
	}
};


class StaticMesh : public RenderMesh
{
private:
	friend class StaticMeshInstance;

public:
	StaticMesh() : RenderMesh() {}


	StaticMesh(const std::string& path) : RenderMesh()
	{
		load(path.c_str());
	}

	/*Returns a new instance. Memory management is up to the user.*/
	StaticMeshInstance* createInstance()
	{
		return new StaticMeshInstance(this);
	}
};