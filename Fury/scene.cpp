#pragma once
#include "scene.h"


SceneNode::SceneNode(SceneNode* parent, Scene* scene) : mType(SCENE)
{
	mTransformable = false;
	setParent(parent);
	mScene = scene;
}


std::vector<Drawable*> mEntities;
std::vector<SceneNode*> mChildren;
SceneNode* mParent;



SceneNode::~SceneNode()
{
	for (size_t i = 0; i < mChildren.size(); ++i)
		delete mChildren[i];
}



Scene::Scene(RenderWindow* window) : mRootNode(new TransformNode(NULL, this)), mActiveCamera(NULL), mWindow(window) {}

Scene::~Scene()
{
	deletePtr(mRootNode);
}

TransformNode* Scene::getRootNode()
{
	return mRootNode;
}

void Scene::clear()
{
	deletePtr(mRootNode);
	mRootNode = new TransformNode(NULL, this);
}

void Scene::draw()
{
	prepare();
	mRootNode->perform(math::mat4());
}

void DrawableNode::perform(const math::mat4& accumulatedMatrix)
 {
	 updateMatrix();

	math::mat4 transformMatrix = m_matrix * accumulatedMatrix;

	if (mDrawable)
		mDrawable->draw(mScene->getWindow(), transformMatrix, *mScene->getActiveCamera());

	for (size_t i = 0; i < mChildren.size(); ++i)
		mChildren[i]->perform(transformMatrix);
}

void CameraNode::perform(RenderWindow* window, const math::mat4& accumulatedMatrix)
{
	math::mat4 transformMatrix = m_matrix * accumulatedMatrix;

	for (size_t i = 0; i < mChildren.size(); ++i)
		mChildren[i]->perform(transformMatrix);
}

LightNode::LightNode(SceneNode* parent, Scene* scene, LightType type) //Type, iterator to light from light list in scene for deletion.
	: TransformNode(parent, scene), needsRecomputing(true)
{
	mType = LIGHT;
	mLightType = type;
	scene->mLights.push_back(this);
	mLightIterator = --scene->mLights.end();
}

LightNode:: ~LightNode()
{
	mParent->deleteChild(this);
	mScene->mLights.erase(mLightIterator);
}