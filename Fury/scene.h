#pragma once
#include "movable.h"
#include <vector>
#include "util.h"
#include "drawable.h"
#include "transformable.h"
#include "camera.h"
#include <list>
#include <stack>
#include "immediateio.h"
#include <assert.h>
/*
The lights are not visible from the tree, but are in it nevertheless. Each light points to its branch,
so that its position is calculated in reverse.
*/

class RenderWindow;
class Scene;

class SceneNode
{
protected:

	enum NodeType {SCENE, TRANSFORM, DRAW, CAMERA, LIGHT, OTHER};

	SceneNode(SceneNode* parent, Scene* scene);

	friend class Scene;

	std::vector<SceneNode*> mChildren;
	SceneNode* mParent;
	Scene* mScene; //The scene where it was created
	NodeType mType;
	bool mTransformable;

	template<class T>
	T* addChild(T* child)
	{
		mChildren.push_back(child);
		return child;
	}

	SceneNode* setParent(SceneNode* parent)
	{
		return mParent = parent;
	}

	
public:
	virtual ~SceneNode();

	virtual void perform(const math::mat4& accumulatedMatrix = math::mat4()) = 0; //Performs some action

	NodeType nodeType()
	{
		return mType;
	}

	bool transformable()
	{
		return mTransformable;
	}

	SceneNode* getParent()
	{
		return mParent;
	}

	/**Tries to delete the element from its children. Use with care.*/
	template<class T>
	void deleteChild(T t)
	{
		for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
			if (*it == t)
			{
				it = mChildren.erase(it); //function returns usable iterator :D
				--it;
			}
	}

};


class TransformNode : public SceneNode, public Transformable
{
protected:
	friend class Scene;

	TransformNode(SceneNode* parent, Scene* scene) : SceneNode(parent, scene)
	{
		mType = TRANSFORM;
		mTransformable = true;
	}

public:

	virtual void perform(const math::mat4& accumulatedMatrix = math::mat4())
	{
		math::mat4 transformMatrix = m_matrix * accumulatedMatrix;

		for (size_t i = 0; i < mChildren.size(); ++i)
			mChildren[i]->perform(transformMatrix);
	}
};


class DrawableNode : public TransformNode
{
protected:
	friend class Scene;

	DrawableNode(SceneNode* parent, Scene* scene) : TransformNode(parent, scene), mDrawable(NULL)
	{
		mType = DRAW;
	}

	Drawable* mDrawable;

public:

	virtual void perform(const math::mat4& accumulatedMatrix = math::mat4());

	void setDrawable(Drawable* drawable)
	{
		mDrawable = drawable;
	}
};



class CameraNode : public TransformNode
{
protected:
	Camera* mCamera;
public:

	CameraNode(SceneNode* parent, Camera* camera, Scene* scene) : TransformNode(parent, scene), mCamera(camera)
	{
		mType = CAMERA;
	}

	virtual void perform(RenderWindow* window, const math::mat4& accumulatedMatrix = math::mat4());
};


/*The scene keeps a separate list of lights, and it's maintained so that the destructor of the LightNode
automatically cleans it up.*/
class LightNode : public TransformNode
{
public:
	enum LightType { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT };
	LightType mLightType;
	std::list<LightNode*>::iterator mLightIterator;
	bool needsRecomputing;

	LightNode(SceneNode* parent, Scene* scene, LightType type); //Type, iterator to light from light list in scene for deletion.


	LightType type()
	{
		return mLightType;
	}

	void computePosition() //Goes down the tree to compute the position of the light at the current frame
	{
		SceneNode* node = mParent;
		m_matrix.reset();

		while (node!=NULL)
		{
			if (node->transformable())
			{
				Transformable* trans = (Transformable*)(node);
				m_matrix = trans->matrix() * m_matrix;
				node = node->getParent();
			}
		}
	}


	//Unregister light from scene:
	virtual ~LightNode();
};

class DirectionalLightNode : public LightNode
{
public:
	DirectionalLightNode(SceneNode* parent, Scene* scene) :
		LightNode(parent, scene, TYPE_DIRECTIONAL) {}
};

class PointLightNode : public LightNode
{
public:
	PointLightNode(SceneNode* parent, Scene* scene) :
		LightNode(parent, scene, TYPE_POINT), mRadius(0) {}

	float mRadius;
};

class SpotLightNode : public LightNode
{
public:
	SpotLightNode(SceneNode* parent, Scene* scene) :
		LightNode(parent, scene, TYPE_SPOT), mRadius(0), mHalfAngle(0) {}

	float mRadius;
	float mHalfAngle;
};


class Scene
{
private:
	TransformNode* mRootNode;
	Camera* mActiveCamera;
	RenderWindow* mWindow;

	std::list<LightNode*> mLights;

	void draw();

	friend class RenderWindow;
	friend class LightNode;

	void prepare()
	{
		for (auto it = mLights.begin(); it != mLights.end(); ++it)
			(*it)->computePosition();
	}

public:

	Scene(RenderWindow* window);
	~Scene();

	TransformNode* getRootNode();

	void clear();

	void setWindow(RenderWindow* window)
	{
		mWindow = window;
	}

	RenderWindow* getWindow()
	{
		return mWindow;
	}

	TransformNode* createTransformNode(SceneNode* parent)
	{
		return parent->addChild(new TransformNode(parent, this));
	}

	DrawableNode* createDrawableNode(SceneNode* parent)
	{
		return parent->addChild(new DrawableNode(parent, this));
	}

	CameraNode* createCameraNode(SceneNode* parent, Camera* camera) //Kinda useless at the moment...
	{
		return parent->addChild(new CameraNode(parent, camera, this));
	}

	DirectionalLightNode* createDirectionalLightNode(SceneNode* parent)
	{
		return parent->addChild(new DirectionalLightNode(parent, this));
	}

	PointLightNode* createPointLightNode(SceneNode* parent)
	{
		return parent->addChild(new PointLightNode(parent, this));
	}

	SpotLightNode* createSpotLightNode(SceneNode* parent)
	{
		return parent->addChild(new SpotLightNode(parent, this));
	}

	void setActiveCamera(Camera* camera)
	{
		mActiveCamera = camera;
	}

	Camera* getActiveCamera()
	{
		return mActiveCamera;
	}


	//The iterator implementation:

	class iterator : public std::iterator < std::forward_iterator_tag, int >
	{
		SceneNode* mRootNode;
		SceneNode* mNode;
		std::stack<size_t> stack; //Keeps track of which paths the iterator took. Index of current child = stack.top()
		bool reachedEnd; //has the end been reached?

		friend class Scene;

	public:
		iterator(SceneNode* node) : mRootNode(node), mNode(node), reachedEnd(false)
		{
			stack.push(0); //start index.
		}

		//++it
		iterator& operator++()
		{
			//If no children left to explore
			while (stack.top() >= mNode->mChildren.size())
			{
				//Are we the parent? If so, reached end.
				if (mNode == mRootNode)
				{
					reachedEnd = true;
					break;
				}
				else //go back one node
				{
					mNode = mNode->mParent;
					stack.pop();
				}
			}
			if (!reachedEnd) //if still any nodes left
			{
				//enter child node while incrementic stack.
				size_t index = stack.top()++; //index to enter, and increment for next time.
				stack.push(0); //index of entered node.
				mNode = mNode->mChildren[index];
			}

			return *this;
		}

		//it++
		iterator operator++(int)
		{
			iterator current = *this;
			++(*this);
			return current;
		}

		bool operator == (const iterator& it) const
		{
			if (mRootNode != it.mRootNode)
			{
				Error("Scene iterators compared from different sources.");
				return false;
			}
			if (mNode == it.mNode && !(reachedEnd^it.reachedEnd))
			{
				return true;
			}
			else
				return false;
		}

		bool operator != (const iterator& it) const
		{
			return !(*this == it);
		}

		//Dereference operator
		SceneNode* operator*() const
		{
			return mNode;
		}

		SceneNode* operator->() const
		{
			return mNode;
		}

	};

	iterator begin() const
	{
		return iterator(mRootNode);
	}

	iterator end() const
	{
		iterator e = begin();
		e.reachedEnd = true;
		return e;
	}

	//erases and returns the next element. Makes input element point to end to help avoid errors.
	iterator erase(iterator& it)
	{
		if (it.mNode == mRootNode)
		{
			Warning("Can not erase root node.");
			return it;
		}
		iterator out = it;
		++out;
		//link parent to the children of the node being deleted, and vice versa.
		it->getParent()->mChildren.insert(it->getParent()->mChildren.end(), it->mChildren.begin(), it->mChildren.end());
		out->setParent(it->getParent());
		return out;
	}

};
